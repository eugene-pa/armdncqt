// поток опроса линии связи
#include <functional>
#include <string>
#include <thread>

#include "mainwindow.h"
#include "kpframe.h"

#include "../spr/raspacker.h"
#include "../spr/station.h"
#include "../common/common.h"
#include "../common/blockingrs.h"
#include "../common/pamessage.h"
#include "threadpolling.h"

extern QString configMain;                                  // строка конфигурации BlockingRS прямого канала
extern QString configRsrv;                                  // строка конфигурации BlockingRS обратного канала

BYTE dataIn [4048];                                         // входные данные
BYTE dataOut[4048];                                         // выходные данные
unsigned int cycles = 0;                                    // счетчик циклов
MainWindow * parent;                                        // родительское окно
QTime       start;                                          // засечка начала цикла

int MakeData();                                             // формирование ответного пакета
void SendMessage (int, void *);                             // сатическая функция отправки сообщения

// поток опроса основного и резервного каналов связи
// long param - здесь - указатель на строку конфигурации config
//              Строку config можно не передаать в поток, а определять из глоб.параметров в самом потоке
//    ВАЖНО:    если строку передавать в поток, в вызывающем потоке то нельзя объявлять строку config
//              как локальный параметр на стеке, так как она будет использоваться здесь в рабочем потоке позже
void ThreadPolling(long param)
{
    parent = (MainWindow*) param;
    Q_UNUSED(param)
    Logger::LogStr ("Поток опроса каналов связи запущен");

    BlockingRS * rs1 = nullptr;
    BlockingRS * rs2 = nullptr;

    // инициируем прямой канал
    if (configMain.length())
    {
        Logger::LogStr ("Прямой канал: " + configMain);
        rs1 = new BlockingRS(configMain);
        rs1->start();
    }
    // инициируем обратный канал
    if (configRsrv.length())
    {
        Logger::LogStr ("Обратный канал: " + configMain);
        rs2 = new BlockingRS(configRsrv);
        rs2->start();
    }

    //int   indxSt = -1;                                                   // индекс актуальной станции опроса
    start = QTime::currentTime();

    // цикл опроса выполняется вплоть до завершения работы модуля
    while (!exit_lock.try_lock_for(chronoMS(100)))
    {
        // если нет коннекта ни в основном, ни в резервном - ждем!
        bool readyMain = rs1 && (rs1->CourierDetect() || true),         // вместо true признак простого порта без несущей
             readyRsrv = rs2 && (rs2->CourierDetect() || true);         // вместо true признак простого порта без несущей
        if (!(readyMain || readyRsrv))
            continue;

        if (actualSt)
        {
            // отображением станции снимаем пинг
            SendMessage (MainWindow::MSG_SHOW_INFO, actualSt->userData);
        }

        // 1. выборка очередной станции
        actualSt = NextSt();                                            // актуальная станция

        // 2. подготовка пакета
        RasPacker pack(actualSt);

        // 2. определение стороны опроса должно быть функцией класса Station
        //    исходные данные - готовность прямого и обратного каналов
        bool back = actualSt->IsBackChannel() && rs2!=nullptr;
        //actualSt->SetBackChannel(actualSt->Addr() & 1);

        // отобразить "пинг" - точку опроса станции (канал, комплект)
        SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);

        bool ret = TryOneChannel(back ? rs2 : rs1, &pack);

        // 4. отправка в нужную сторону, ожидание и прием ответного пакета
        // 5. анализ
        // 6. при необходимости отправка в противоположную сторону, ожидание и прием ответного пакета

        try
        {
            //int indx = 0;
            //int ch;
            // ждем маркер
/*
            if ((ch = rs1->GetCh()) != SOH)               // используем функцию без исключений пока ждем маркер
                continue;
*/

        }
        catch (...)
        {
            continue;
        }
    }
    exit_lock.unlock();
    if (rs1 != nullptr)
    {
        rs1->Close();
        delete rs1;
    }
    if (rs2 != nullptr)
    {
        rs2->Close();
        delete rs2;
    }

    Logger::LogStr ("Поток опроса каналов связи завершен");
}

// опрос по заданному каналу
bool TryOneChannel(BlockingRS * rs, RasPacker* data)
{
    if (rs == nullptr)
        return false;
    rs->Send(data, data->Length());
    SendMessage (MainWindow::MSG_SHOW_SND, data);
    bool ret = GetData(rs);
}

// прием данных
bool GetData(BlockingRS * rs)
{
    int indx = 0;
    int ch;
    // ждем маркер
    // время ожидания можно передать как параметр, или использовать умолчание RsAsinc::timeWaiting = 100мс,
    // которое можно изменить функцией SetTimeWaiting(int ms)
    // таким образом, имеем единственный таймаут, управляющий работой приема, причем обрабатываемый внутри RsAsinc, а не ОС
    try
    {
        while ((ch = rs->GetChEx()) != SOH)             // ожидаем маркер
            ;

        dataIn[indx++] = SOH;
        // прием длины
        dataIn[indx++] = rs->GetChEx();                  // здесь и далее более лаконичный код с использованием исключения при отсутствии данных
        dataIn[indx++] = rs->GetChEx();
        int l = (dataIn[indx-1] << 8) | dataIn[indx-2];

        // прием тела пакета
        for (int i=0; i<l; i++)
        {
            dataIn[indx++] = rs->GetChEx();
        }

        // прием CRC и EOT
        dataIn[indx++] = rs->GetChEx();
        dataIn[indx++] = rs->GetChEx();
        dataIn[indx++] = rs->GetChEx();

        // проверка CRC
        WORD crc = (dataIn[indx-2] << 8) | dataIn[indx-3];
        WORD crcreal = GetCRC(&dataIn, l + LEN_HEADER);
        if (crc != crcreal)
        {
            int a = 99;
        }
        ((RasPacker *)&dataIn)->st = actualSt;
        SendMessage (MainWindow::MSG_SHOW_RCV, dataIn);
    }
    catch (...)
    {
        return -1;
    }

    return true;
}





// получить след.станцию для опроса
// функция должна обеспечивать приоритет станциям, для которых есть директивы/ТУ/ОТУ
Station * NextSt()
{
    if (++RasPacker::indxSt >= Station::StationsOrg.size())
    {
        RasPacker::indxSt = 0;
        parent->setCycles(++cycles);
        parent->setPeriod((int)(start.msecsTo(QTime::currentTime())));
        start = QTime::currentTime();
    }
    return Station::StationsOrg[RasPacker::indxSt];
}




#ifdef DBG_INCLUDE
// пример ответного пакета
int MakeData()
{
    dataOut[0] = SOH;
    dataOut[1] = 3;
    dataOut[2] = 0;
    dataOut[3] = 0;
    dataOut[4] = dataIn[3];
    dataOut[5] = dataIn[5];
    WORD crc = GetCRC(dataOut, 6);
    dataOut[6] = crc & 0xff;
    dataOut[7] = (crc >> 8) & 0xff;
    dataOut[8] = EOT;
    return 9;
}
#endif // #ifdef DBG_INCLUDE
