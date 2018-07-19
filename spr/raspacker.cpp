#include "raspacker.h"
#include "station.h"

int  RasPacker::indxSt = -1;                            // индекс актуальной станции опроса

RasPacker::RasPacker(class Station * st)
{
    marker      = SOH;                                  // маркер
    length      = (WORD)(long)(data-&dst);              // длина пакета (все после себя, исключая CRC и EOT)
    dst         = st ? st->Addr() : 0;                  // адрес назначения
    src         = CpuAddress;                           // адрес источника

    // формирование счетчика сеансов должно быть интеллектуальным с учетом существующих алгоритмов полного опроса
    // на старых КП полный опрос обеспечивается нулевым значением сеанса, на новых КП используется инкремент счетчика на 2
    seans       = st->GetSeans();                       // сеанс
    if (st->IsFullPolling())
    {
        seans = st->Kp2007() ? st->IncSeanc() : 0;
    }

    memset (data, 0, sizeof(data));                     // очистка поля
    this->st = st;

    if (st)
    {
        // Условия для передачи метки времени:
        // - новый КП
        // - минута или более с момента последней синхронизации
        // - пустой блок для КП (нет ТУ/ОТУ или директив)
        if (st->Kp2007() && st->lastTimeSinchro.secsTo(QDateTime::currentDateTime()) >= 60 && st->rasDataOut->Length()==0)
        {
            BYTE buf[sizeof(TuPackTimeSet)];
            memmove(buf, TuPackTimeSet, sizeof(buf));
            * (qint32 *)(buf + sizeof (buf) - 4) = (qint32) QDateTime::currentDateTime().toTime_t();
            st->AcceptDNC((RasData*)&buf);
            st->lastTimeSinchro = QDateTime::currentDateTime();
        }

        // на время работы блокируем доступ к rasDataOut; разблокировка выполняется в деструкторе при выходе из блока
        std::lock_guard <std::mutex> locker(st->rasDataOutLock);
        int l = std::min((int)st->rasDataOut->Length(), (int)sizeof(data));
        if (l)
        {
            memmove(data, st->rasDataOut, l);           // переносим данные
            st->rasDataOut->Clear();                    // обнуляем блок
            length += l;                                // наращиваем длину пакета на длину блока данных
        }
        addCRC (this, length + LEN_HEADER);             // подсчет CRC
        data[l+2] = EOT;                                // запись EOT

    }

}



// ------------------------------------------------------------------------------------------------------------------------
// Класс RasData
// длина заданного блока
int RasData::Length (int n)
{
    switch (n)
    {
        case SYSBLCK  : return LengthSys  ();
        case TUTSBLCK : return LengthTuts ();
        case OTUBLCK  : return LengthOtu  ();
        case DIAGBLCK : return LengthDiag ();
        default       : return 0;
    }
}


// лина блоков, начиная с заданного
int RasData::LengthFrom (int n)
{
    int l = 0;
    for (int i=n; i<DIAGBLCK; i++)
    {
        l += Length(i);
    }
    return l;
}


// указатель на заданный блок
BYTE * RasData::PtrBlck (int n)
{
    switch (n)
    {
        case SYSBLCK  : return PtrSys  ();
        case TUTSBLCK : return PtrTuTs ();
        case OTUBLCK  : return PtrOtu  ();
        case DIAGBLCK : return PtrDiag ();
        default       : return PtrDiag () + LengthDiag();       // указатель на свободное место
    }
}

// побайтное копирование класса
void RasData::Copy(RasData* pSrc)
{
    memmove(this, pSrc, sizeof(RasData));
}


// суммирование инфо-блоков (если не успели отправить старую посылку)
void RasData::Append(RasData* pSrc, Station* st)
{
    // если данные устарели, их надо удалить
//    if (Length() && st->tuTime.secsTo(QDateTime::currentDateTime()) > 30)
//        Clear();
    st->tuTime = QDateTime::currentDateTime();                  // засечка
    // директивы можно суммировать
    // ТУ можно суммировать
    // ОТУ нельзя суммировать, игнорируем старую информацию
    // поле диагностики не используется при передаче в КП
    if (pSrc->LengthSys())
    {
        AppendBlock(pSrc, SYSBLCK);
    }
    if (pSrc->LengthTuts())
    {
        AppendBlock(pSrc, TUTSBLCK);
    }
    if (pSrc->LengthOtu())
    {
        DeleteBlock(OTUBLCK);
        AppendBlock(pSrc, OTUBLCK);
    }
}

// объединить заданные блоки (присоединяем к существующему новую инфу)
void RasData::AppendBlock(RasData* pSrc, BYTE n)
{
    int l    = Length(n);                                       // исходная длина
    int ladd = pSrc->Length(n);                                 // доп.длина
    BYTE * org = PtrBlck(n+1);                                  // сдвигаем со следующего блока
    BYTE * dst = org + ladd;                                    // смещаем на доп.длину
    memmove (dst, org, LengthFrom(n+1));                        // сдвигаем все блоки (надо ввсети ограничение на длину)
    memmove (org, pSrc->PtrBlck(n), ladd);                      // дописываем сист.инфу
    SetBlockLen(n, l + ladd);                                   // новая длина
    // Logger::LogStr();
}


// очистить информацию по блоку
void RasData::DeleteBlock(BYTE n)
{
    if (Length(n))
    {
        BYTE * src = PtrBlck(n+1);                              // источник - след.блок
        BYTE * dst = PtrBlck(n);                                // назначение - данный блок
        memmove (dst, src, LengthFrom(n+1));                    // сдвигаем все блоки (надо ввсети ограничение на длину)
        SetBlockLen(n, 0);                                      // бнуляем длину
    }
}

// записать длину блока с учетом битов расширения
// n - номер блока (0/1/2/3)
// l - длина блока
void RasData::SetBlockLen (WORD n,int l)
{
    extLength &= ~(MSK_LEN_EXT << (n*2));                       // сбросить биты расширений
    extLength |= ((l>>8) & MSK_LEN_EXT)<<(n*2);                 // записать биты расширений
    * (((BYTE *)this) + n + 1) = (BYTE) (l & 0xff);             //
}


void RasData::Clear()                                           // очистка (обнуление) при отсутствии связи
{
    memset (this, 0, sizeof(RasData));
}


// бщая длина данных
// если длины всех блоков = 0, то общая длина = 0, иначе, прибавляем 6 байт заголовка
int RasData::Length ()
{
    int lsum = LengthSys () + LengthTuts() + LengthOtu () + LengthDiag();
    return  lsum ? lsum + 6 : 0;
}


// пакет содержит ТОЛЬКО широковещательную рассылку УПОК
bool RasData::IsBroadcast()
{
    return      LengthSys ()==0                                 // нет системной информации
            &&  LengthTuts()==0                                 // нет ТУ
            &&  LengthOtu ()!=0                                 // есть ОТУ
            &&  LengthOtu() > 7                                 // ОТУ не СПОК
            &&  ((UpokFromData*)PtrOtu())->IsBroadcast();       // блок ОТУ широковещательный
}

// пакет содержит данные для ОМУЛ, требует приоритетного ответа
//bool RasData::IsTuOmul()
//{
//    return LengthOtu () && LengthOtu () <= 7;
//}
