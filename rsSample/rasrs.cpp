#include "rasrs.h"

RasRS::RasRS(QObject *parent, BYTE marker, int maxlength) : BlockingRs(parent, marker, maxlength)
{
    seans = 0;
}

RasRS::~RasRS()
{

}

// основной цикл
void RasRS::mainLoop()
{
    while (!quit)
    {
        pSerial->write(prepareData());                      // запрос
        if (quit)
            break;

        QByteArray data = readData(*pSerial);               // прием отклика
        if (quit)
            break;

        //  ....                                            // обработка

        emit dataready(data);                           // уведомление
    }
}


// формирование пакета для передачи
QByteArray RasRS::prepareData()
{
    QByteArray data;
    data.append(01);                                        // маркер
    data.append(03);                                        // длина
    data.append((char)0);
    data.append(01);                                        // приемник
    data.append((char)0);                                   // источник
    data.append(seans++);                                   // сеанс
    addCRC(data);                                           // CRC
    data.append(04);                                        // маркер конца пакета
    return data;
}
