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
#include "../common/archiver.h"
#include "threadpolling.h"

// статически объявленные переменные, используемые в рабочем потоке
static BYTE dataIn [MAX_LINE_DATA_LEN*2];                   // входные данные COM-порта
       std::queue <unsigned char> dataInNet;                // входные данные, полученные по сети хранятся в очереди
       std::mutex                 mtxDataNet;               // синхронизация доступа к очереди dataInNet из основного и рабочего потоков

//BYTE dataOut[4048];                                       // выходные данные
unsigned int cycles = 0;                                    // счетчик циклов всех станций
QTime       start;                                          // засечка начала цикла
//static MainWindow * parent;                                 // родительское окно

// реализация тайм-аутов ожидания квитанции
bool                    armAcked;                           // получена квитанция АРМ ДНЦ
std::condition_variable waterAck;                           // условие ожидания квитанции АРМ ДНЦ
std::condition_variable waterNet;                           // условие ожидания отклика КП по сети
std::mutex              mtxWater;                           // мьютекс для организации ожидания поступления данных (квитанций, отклика - все на одном мьютексе)
static int              ackTimeout = 1000;                  // максимальное время ожидания квитанции АРМ ДНЦ
static int              netTimeout = 100;                   // максимальное время ожидания отклика по сети
static ArhWriter arhWriter("log", "@_ras");                 // бинарный архив

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
    Q_UNUSED (param)
    Logger::LogStr ("Поток опроса каналов связи запущен");

    //parent = (MainWindow*) param;

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
        rs2->SetTimeWaiting(200);
        rs2->start();
    }

    start = QTime::currentTime();

    // ------------------------------------------------------------------------------------------------------------------
    // цикл опроса выполняется вплоть до завершения работы модуля
    // ожидание между опросом смежных станций должно быть минимальным (ожидание между ПРД и ПРМ задается задержками, см.TryOneChannel)
    // но: надо реализовать механизм ожидания квитанции
    while (!exit_lock.try_lock_for(chronoMS(1)))
    {
        // если нет коннекта ни в основном, ни в резервном - ждем!
        bool readyMain = rs1 && (rs1->CourierDetect() || true),         // вместо true признак простого порта без несущей
             readyRsrv = rs2 && (rs2->CourierDetect() || true);         // вместо true признак простого порта без несущей

        // поддерживаем актуальное состояние каналов
        Station::MainLineCPU = rs1==nullptr ? 0 : !rs1->IsOpen() ? -1 : rs1->CourierDetect() ? 2 : 1;    // -1(3)/0/1/2 (отказ/откл/WAITING/OK)
        Station::RsrvLineCPU = rs2==nullptr ? 0 : !rs2->IsOpen() ? -1 : rs2->CourierDetect() ? 2 : 1;

        if (!(readyMain || readyRsrv) && !MainWindow::IsNetSupported()) // если нет готовности портов и не описано сетевое подключение - ожидание
            continue;

        if (actualSt)
            SendMessage (MainWindow::MSG_SHOW_INFO, actualSt->userData);// отображением станции снимаем пинг

        // выключенные станции не опрашиваем
        actualSt = NextSt();                                            // актуальная станция
        if (!actualSt->Enable())                                        // если выключена из опроса - пропускаем
            continue;

        // если была отправка в АРМ, ожидаем квитанцию от АРМ ДНЦ не более заданного времени
        if (!armAcked && g_rqAck)
        {
            std::unique_lock<std::mutex> lck(mtxWater);
            waterAck.wait_for(lck, std::chrono::milliseconds(ackTimeout));
        }

        actualSt->SetKpResponce(false);                                 // пока отклика нет
        RasPacker pack(actualSt);                                       // подготовка пакета
        bool back = actualSt->IsBackChannel();                          // back - актуальная сторона опроса
        SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);    // отобразить "пинг" - точку опроса станции (канал, комплект)

        arhWriter.Save(&pack, pack.Length(), 2);

        Station * st = nullptr;                                         // станция отклика

        // если задан сетевой опрос - опросить по сети, иначе - по COM-портам
        // можно оставить опрос по COM-портам при отсутствии отклика из сети
        if (MainWindow::IsNetSupported())                               // работаем по сети
        {
            SendMessage (MainWindow::MSG_SND_NET, &pack);
            std::unique_lock<std::mutex> lck(mtxWater);
            std::cv_status ret = waterNet.wait_for(lck, std::chrono::milliseconds(netTimeout));
            if (ret != std::cv_status::timeout)
            {
                if (GetData(nullptr))                                  // приняты данные, обработать
                    st = ((RasPacker *)&dataIn)->st;
            }
        }
        else
        if ( (st = TryOneChannel(back ? rs2 : rs1, &pack)) == nullptr)  // работаем по СОМ-портам
        {
            actualSt->SetBackChannel(back = !back);                     // если не было отклика - пробуем с другой стороны
            SendMessage (MainWindow::MSG_SHOW_PING, actualSt->userData);
            st = TryOneChannel(back ? rs2 : rs1, &pack);
        }

        // все, что могли, опросили, оцениваем результат
        if (st != nullptr)                                              // если был отклик - пометить!
        {
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

            st->SetKpResponce(true);
        }
        else
        {
            // уведомить об ошибке
            SendMessage (MainWindow::MSG_ERR, actualSt);
            actualSt->GetSysInfo()->SetLineStatus(LineTimeOut);
            armAcked = true;                                            // не уведомляли АРМ, значит не ждем квитанцию
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
    SleepMS(100);
    SendMessage (MainWindow::MSG_SHOW_SND, data);
    return GetData(rs) ? ((RasPacker *)&dataIn)->st : nullptr;
}

// получить байт сетевых данных
int GetChNet()
{
    bool nodata = false;
    int  bt;
    {
        std::lock_guard <std::mutex> locker(mtxDataNet);
        if (dataInNet.empty())
            nodata = true;
        else
        {
            bt = dataInNet.front();
            dataInNet.pop();
        }
    }
    if (nodata)
        throw RsException();
    return bt;
}

// получить байт данных или из СОМ-порта или сетевых
// rs - указатель на СОМ-порт, если нулевой - берем сетевые данные
int GetChar(BlockingRS * rs)
{
    return rs == nullptr ? GetChNet() : rs->GetChEx();
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
        while ((ch = GetChar(rs)) != SOH)                       // ожидаем маркер
            ;

        dataIn[indx++] = SOH;
        // прием длины
        dataIn[indx++] = GetChar(rs);                           // младший байт длины
        dataIn[indx++] = GetChar(rs);                           // старший байтдлины
        int l = (dataIn[indx-1] << 8) | dataIn[indx-2];

        // прием тела пакета
        for (int i=0; i<l; i++)
        {
            dataIn[indx++] = GetChar(rs);
        }

        // прием CRC и EOT
        dataIn[indx++] = GetChar(rs);
        dataIn[indx++] = GetChar(rs);
        dataIn[indx++] = GetChar(rs);

        arhWriter.Save(&dataIn, indx, 1);                        // сохранить полученный пакет

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
            SendMessage (MainWindow::MSG_ERR_CRC, dataIn);      // ошибка CRC
            return false;
        }
        else
        if (l < 9)
        {
            if (st)
                st->GetSysInfo()->SetLineStatus(LineFormat);
            SendMessage (MainWindow::MSG_ERR_FORMAT, dataIn);   // ошибка формата
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
        cycles++;
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
