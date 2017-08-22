// поток опроса линии связи
#include <functional>
#include <string>
#include <thread>

#include "threadpolling.h"
#include "../common/common.h"
#include "../common/blockingrs.h"
#include "../common/pasender.h"
#include "../common/pamessage.h"

BYTE dataIn [4048];                                         // входные данные
BYTE dataOut[4048];                                         // выходные данные
int MakeData();                                             // формирование ответного пакета

//
//extern PaSender paSender;
//std::function<void(PaSender&, class PaMessage *)> rsNotifier;


// поток опроса
// long param - здесь - указатель на класс RsAsinc (так как приложение консольное,
// класс RsAsinc должен быть создан в основном потоке, иначе не будет работать
// В GUI приложениях класс RsAsinc логичнее создавать прямо в потоке
void ThreadPolling(long param)
{
    Log(L"Старт потока ThreadPolling");
    QString config = QString::fromStdWString(*(std::wstring *)param);
    BlockingRS rs(config);
    rs.start();

    while (!exit_lock.try_lock_for(chronoMS(10)))
    {
        try
        {
            int indx = 0;
            int ch;
            // ждем маркер
            // время ожидания можно передать как параметр, или использовать умолчание RsAsinc::timeWaiting = 100мс,
            // которое можно изменить функцией SetTimeWaiting(int ms)
            // таким образом, имеем единственный таймаут, управляющий работой приема, причем обрабатываемый внутри RsAsinc, а не ОС
            // ВАЖНО: оглядываясь на старую реализацию Windows следует заметить, что, так как чтение было посимвольное,
            // в структуре COMMTIMEOUTS значение имела скорее всего только одна задержка на прием байта, все остальные
            // должны работать при блочном приеме/передаче...
            if ((ch = rs.GetCh()) != SOH)               // используем функцию без исключений пока ждем маркер
                continue;
            dataIn[indx++] = ch;

            // прием длины
            dataIn[indx++] = rs.GetChEx();              // здесь и далее более лаконичный код с использованием исключения при отсутствии данных
            dataIn[indx++] = rs.GetChEx();
            int l = (dataIn[LENGTH_OFFS+1] << 8) | dataIn[LENGTH_OFFS];

            // прием тела пакета
            for (int i=0; i<l; i++)
            {
                dataIn[indx++] = rs.GetChEx();
            }

            // прием CRC и EOT
            dataIn[indx++] = rs.GetChEx();
            dataIn[indx++] = rs.GetChEx();
            dataIn[indx++] = rs.GetChEx();

            // проверка CRC
            WORD crc = (dataIn[indx-2] << 8) | dataIn[indx-3];
            WORD crcreal = GetCRC(dataIn, l + LEN_HEADER);
            if (crc != crcreal)
            {
                SendMessage(new PaMessage(PaMessage::srcActLine, L"CRC error!", PaMessage::eventError, PaMessage::stsErrCRC));
            }
            else
            {
                std::wstring msg = L"Прием:  ";
                msg += GetHexW(dataIn, indx).c_str();
                SendMessage (new PaMessage(PaMessage::srcActLine, msg, PaMessage::eventReceive, PaMessage::stsOK, dataIn, indx));

                // формирование и передача ответного пакета
                int l = MakeData();
                rs.Send(dataOut, l);
            }

        }
        catch (...)
        {
            continue;
        }
    }
    exit_lock.unlock();
    rs.Close();
    Log (L"Поток опроса линни связи завершен!");
}

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
