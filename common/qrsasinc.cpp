#include "qrsasinc.h"

extern std::timed_mutex exit_lock;                                     // блокировка до выхода
WORD GetCRC (BYTE *buf,WORD Len);
std::wstring GetHexW(void *data, int length);

QRsAsinc::QRsAsinc(QString ini, QObject *parent)
{
    Q_UNUSED(parent)
    //rsNotifier = static_cast <std::function<void(PaSender&, class PaMessage *)>> (notifier);
    config = ini;
}

void QRsAsinc::process()
{
    //if (rsNotifier != nullptr)
    //    rsNotifier(paSender, new PaMessage(PaMessage::srcActLine, PaMessage::typeTrace, PaMessage::stsOK, L"Поток опроса линни связи запущен!"));

    prs = new RsAsinc (config);
    int l = MakeData();
    prs->Send(dataOut, l);
    RsAsinc& rs = * prs;
    while (!exit_lock.try_lock_for(std::chrono::milliseconds(100)))
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
                if (rsNotifier != nullptr)
                {
                    std::wstring msg = L"Прием:  ";
                    msg += GetHexW(dataIn, indx).c_str();
//                  rsNotifier(paSender, new PaMessage(PaMessage::srcActLine, PaMessage::typRcv, PaMessage::stsOK, msg, dataIn, indx));
                }

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
//    if (rsNotifier != nullptr)
//        rsNotifier(paSender, new PaMessage(PaMessage::srcActLine, PaMessage::typeTrace, PaMessage::stsOK, L"Поток опроса линни связи завершен!"));

}

// пример ответного пакета
int QRsAsinc::MakeData()
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
