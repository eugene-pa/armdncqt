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

// статически объявленные переменные, используемые в рабочем потоке
static BYTE dataIn [4048];                                  // входные данные
//BYTE dataOut[4048];                                       // выходные данные
static unsigned int cycles = 0;                             // счетчик циклов
static MainWindow * parent;                                 // родительское окно
static QTime       start;                                   // засечка начала цикла


// Взаимодействие рабочего потока с главным окном выполняется в нестандартной для QT технологии:
// путем вызова статической функции главного окна, которая генерит сигнал
// Это сделано по той причине, что генерация сигналов QT возможна ТОЛЬКО из классов, наследуемых от QObject,
// что накладывает ряд ограничений, в частности по модели организации многопоточности, конфликтующих с std::thread
// поэтому, вместо генерации нескольких сигналов поток вызывает одну функцию с разными сообщениями
// В принципе, можно было бы попробовать и стандартную QT организацию этих потоков, насколько я понимаю
//
// поток опроса основного и резервного каналов связи
// long param - здесь - указатель на строку конфигурации config
//              Строку config можно не передаать в поток, а определять из глоб.параметров в самом потоке
//    ВАЖНО:    если строку передавать в поток, в вызывающем потоке то нельзя объявлять строку config
//              как локальный параметр на стеке, так как она будет использоваться здесь в рабочем потоке позже
void ThreadPolling(long param)
{
    Logger::LogStr ("Поток опроса каналов связи запущен");

    parent = (MainWindow*) param;

    BlockingRS * rs1 = nullptr;
    BlockingRS * rs2 = nullptr;

    // инициируем прямой канал
    if (configMain.length())
    {
        Logger::LogStr ("Прямой канал: " + configMain);
        rs1 = new BlockingRS(configMain);
        rs1->SetTimeWaiting(200);
        rs1->start();
    }

    // инициируем обратный канал
    if (configRsrv.length())
    {
        Logger::LogStr ("Обратный канал: " + configMain);
        rs2 = new BlockingRS(configRsrv);
        rs1->SetTimeWaiting(200);
        rs2->start();
    }

    start = QTime::currentTime();

    // ------------------------------------------------------------------------------------------------------------------
    // цикл опроса выполняется вплоть до завершения работы модуля
    while (!exit_lock.try_lock_for(chronoMS(1)))
    {
        // если нет коннекта ни в основном, ни в резервном - ждем!
        bool readyMain = rs1 && (rs1->CourierDetect() || true),         // вместо true признак простого порта без несущей
             readyRsrv = rs2 && (rs2->CourierDetect() || true);         // вместо true признак простого порта без несущей

        // поддерживаем актуальное состояние каналов
        Station::MainLineCPU = rs1==nullptr ? 0 : !rs1->IsOpen() ? -1 : rs1->CourierDetect() ? 2 : 1;    // -1(3)/0/1/2 (отказ/откл/WAITING/OK)
        Station::RsrvLineCPU = rs2==nullptr ? 0 : !rs2->IsOpen() ? -1 : rs2->CourierDetect() ? 2 : 1;

        if (!(readyMain || readyRsrv))
            continue;

        if (actualSt)
            SendMessage (MainWindow::MSG_SHOW_INFO, actualSt->userData);// отображением станции снимаем пинг

        // выключенные станциине опрашиваем
        actualSt = NextSt();                                            // актуальная станция
        if (!actualSt->Enable())                                        // если выключена из опроса - пропускаем
            continue;

        actualSt->SetKpResponce(false);                                 // пока отклика нет

        RasPacker pack(actualSt);                                       // подготовка пакета

        bool back = actualSt->IsBackChannel();                          // back - актуальная сторона опроса
        SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);    // отобразить "пинг" - точку опроса станции (канал, комплект)

        Station * st = nullptr;                                         // станция отклика
        if ( (st = TryOneChannel(back ? rs2 : rs1, &pack)) == nullptr)
        {
            // если нет отклика - пробуем с другой стороны
            actualSt->SetBackChannel(back = !back);
            SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);
            st = TryOneChannel(back ? rs2 : rs1, &pack);
        }
        if (st != nullptr)                                              // если был отклик - пометить!
        {
            st->SetKpResponce(true);

            // обработать данные
            RasPacker * pack = (RasPacker *)dataIn;
            if (pack->dst != 0)
            {
                // получатель - не станция связи
                continue;
            }
            if (st != actualSt)
            {
                // данные от другой станции
                // continue;
            }

            // получили данные, передаем в АРМ ДНЦ
/*
            RasData * data = (RasData *)pack->data;
            BYTE * ptr1 = (BYTE *)st->GetSysInfo(false);
            BYTE * ptr2 = data->PtrSys();
            memcpy(ptr1, ptr2, 15);
            //memcpy(st->GetSysInfo(true ), (RasData *)pack->data[15], 15);
*/
        }
    }
    // ------------------------------------------------------------------------------------------------------------------


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
Station * TryOneChannel(BlockingRS * rs, RasPacker* data)
{
    if (rs == nullptr)
        return nullptr;
    rs->Send(data, data->Length());
    //SleepMS(100);
    SendMessage (MainWindow::MSG_SHOW_SND, data);
    return GetData(rs) ? ((RasPacker *)&dataIn)->st : nullptr;
}

// прием данных во входной массив dataIn из заданного канала
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
        dataIn[indx++] = rs->GetChEx();                 // младший байт длины
        dataIn[indx++] = rs->GetChEx();                 // старший байтдлины
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

        // хочу вернуть справочник станции, от которой получены данные
        // использование актуальной станции некорректно, так как ответ может прийти от другой станции
        Station * st = Station::GetByAddr(((RasPacker *)&dataIn)->src);
        ((RasPacker *)&dataIn)->st = st;

        // проверка CRC
        WORD crc = (dataIn[indx-2] << 8) | dataIn[indx-3];
        WORD crcreal = GetCRC(&dataIn, l + LEN_HEADER);
        if (crc != crcreal)
        {
            if (st)
                st->GetSysInfo()->SetLineStatus(LineCRC);
            SendMessage (MainWindow::MSG_ERR_CRC, dataIn);
            return false;
        }
        else
        if (l < 9)
        {
            if (st)
                st->GetSysInfo()->SetLineStatus(LineFormat);
            SendMessage (MainWindow::MSG_ERR_CRC, dataIn);
        }
        else
        {
            // нормальный прием
            if (st)
                st->GetSysInfo()->SetLineStatus(LineOK);
            SendMessage (MainWindow::MSG_SHOW_RCV, dataIn); // уведомление о приеме
        }
    }
    catch (...)
    {
        if (actualSt)
            actualSt->GetSysInfo()->SetLineStatus(LineTimeOut);
        SendMessage (MainWindow::MSG_ERR_TIMEOUT, dataIn);
        return false;
    }

    return true;
}





// получить след.станцию для опроса
// функция должна обеспечивать приоритет станциям, для которых есть директивы/ТУ/ОТУ
Station * NextSt()
{
    if (++RasPacker::indxSt >= (int)Station::StationsOrg.size())
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
