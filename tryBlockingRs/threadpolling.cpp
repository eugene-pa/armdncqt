#include <string>
#include "../common/blockingrs.h"
#include "../common/common.h"

const WORD LENGTH_OFFS		= 1;
const WORD DST_OFFS			= 3;
const WORD SRC_OFFS			= 4;
const WORD SEANS_OFFS		= 5;
const WORD FRAME_DATA_OFFS	= 6;

const int  LEN_CTRL_INFO    = 6;								// длина обрамления (маркер, длина, CRC, EOT)
const int  LEN_HEADER       = 3;								// длина заголовка (маркер+поле длины)
const int  CRC_SIZE         = 2;								//
const BYTE SOH              = 1;
const BYTE EOT              = 4;
const BYTE CpuAddress       = 0;
BYTE dataIn [4048];                                         // входные данные
BYTE dataOut[4048];                                         // выходные данные

int MakeData();

void ThreadPolling(long param)
{
    Log(L"Старт потока ThreadPolling");
    QString config = QString::fromStdWString(*(std::wstring *)param);
    BlockingRS rs(config);
    rs.start();

    while (!exit_lock.try_lock_for(std::chrono::milliseconds(10)))
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
//                if (rsNotifier != nullptr)
//                    rsNotifier(paSender, new PaMessage(PaMessage::srcActLine, PaMessage::typeTrace, PaMessage::stsErrCRC, L"CRC error!"));

            }
            else
            {
/*
                if (rsNotifier != nullptr)
                {
                    wstring msg = L"Прием:  ";
                    msg += GetHexW(dataIn, indx).c_str();
                    rsNotifier(paSender, new PaMessage(PaMessage::srcActLine, PaMessage::typRcv, PaMessage::stsOK, msg, dataIn, indx));
                }
*/
                // формирование и передача ответного пакета
                // передача работает, но: в отладчике получаю диагностическое сообщение:
                // Invalid parameter passed to C runtime function.
                // ПРИЧИНА: использование класса rs не в том потоке, гдн он был создан
                int l = MakeData();
                rs.Send(dataOut, l);
            }

        }
        catch (...)
        {
            continue;
        }
    }
    rs.Close();
    QThread::sleep(1);
    Log(L"Завершение потока ThreadPolling");
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
