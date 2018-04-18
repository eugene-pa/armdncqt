// поток опроса линии связи
#include <functional>
#include <string>
#include <thread>

#include "mainwindow.h"
#include "kpframe.h"

#include "../spr/station.h"
#include "../common/common.h"
#include "../common/blockingrs.h"
#include "../common/pamessage.h"
#include "threadpolling.h"

#pragma pack(1)

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
    while (!exit_lock.try_lock_for(chronoMS(500)))
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
        RasHeader pack(actualSt);

        // 2. определение стороны опроса должно быть функцией класса Station
        //    исходные данные - готовность прямого и обратного каналов
        bool back = actualSt->IsBackChannel();
        //actualSt->SetBackChannel(actualSt->Addr() & 1);

        // отобразить "пинг" - точку опроса станции (канал, комплект)
        SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);

        // 3. подготовка пакета


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

// получить след.станцию для опроса
// функция должна обеспечивать приоритет станциям, для которых есть директивы/ТУ/ОТУ
Station * NextSt()
{
    if (++RasHeader::indxSt >= Station::StationsOrg.size())
    {
        RasHeader::indxSt = 0;
        parent->setCycles(++cycles);
        parent->setPeriod((int)(start.msecsTo(QTime::currentTime())));
        start = QTime::currentTime();
    }
    return Station::StationsOrg[RasHeader::indxSt];
}


BYTE RasHeader::counter = 0;                            // циклический счетчик сеансов
int  RasHeader::indxSt;                                 // индекс актуальной станции опроса
RasHeader::RasHeader(class Station * st)
{
    marker      = SOH;                                  // маркер
    length      = (WORD)(long)(data-&dst);               // длина пакета (все после себя, исключая CRC и EOT)
    dst         = st ? st->Addr() : 0;                  // адрес назначения
    src         = CpuAddress;                           // адрес источника

    // формирование счетчика сеансов должны быть интеллектуальным с учетом существующих алгоритмов полного опроса
    // на старых КП полный опрос обеспечивается нулевым значением сеанса, на новых КП используется инкремент счетчика на 2
    seans       =++counter;                             // сеанс

    memset (data, 0, sizeof(data));                     // очистка поля

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
        data[l+2] = EOT;                            // запись EOT
        addCRC ((BYTE *)&dst, length);                  // подсчет CRC

    }

}
#pragma pack()

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
