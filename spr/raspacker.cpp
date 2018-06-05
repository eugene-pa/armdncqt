#include "raspacker.h"
#include "station.h"

BYTE RasPacker::counter = 0;                            // циклический счетчик сеансов
int  RasPacker::indxSt = -1;                            // индекс актуальной станции опроса
RasPacker::RasPacker(class Station * st)
{
    marker      = SOH;                                  // маркер
    length      = (WORD)(long)(data-&dst);              // длина пакета (все после себя, исключая CRC и EOT)
    dst         = st ? st->Addr() : 0;                  // адрес назначения
    src         = CpuAddress;                           // адрес источника

    // формирование счетчика сеансов должны быть интеллектуальным с учетом существующих алгоритмов полного опроса
    // на старых КП полный опрос обеспечивается нулевым значением сеанса, на новых КП используется инкремент счетчика на 2
    seans       = ++counter;                            // сеанс

    memset (data, 0, sizeof(data));                     // очистка поля
    this->st = st;

    if (st)
    {
        // на время работы блокируем доступ к DataToKp; разблокировка выполняется в деструкторе при выходе из блока

        std::lock_guard <std::mutex> locker(st->DataToKpLock);
        int l = std::min((int)st->DataToKpLenth, (int)sizeof(data));
        if (st->DataToKpLenth)
        {
            memmove(data,st->DataToKp, l);              // переносим данные
            st->DataToKpLenth = 0;                      // обнуляем длину
            length += l;                                // наращиваем длину пакета на длину блока данных
        }
        addCRC (this, length + LEN_HEADER);             // подсчет CRC
        data[l+2] = EOT;                                // запись EOT

    }

}

// побайтное копирование класса
void RasData::Copy(RasData* p)
{
    memmove(this, p, sizeof(RasData));
}

void RasData::Clear()                                   // очистка (обнуление) при отсутствии связи
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
