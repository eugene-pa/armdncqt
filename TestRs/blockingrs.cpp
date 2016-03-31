#include "blockingrs.h"

BlockingRs::BlockingRs(QObject *parent, BYTE marker, int maxlength) : QThread(parent)
{
    tm = { 100,0, 400, 1, 500 };
    quit = false;
    this->parent = parent;                                  // родитель
    this->marker = marker;                                  // маркер или 0
    this->maxlength = maxlength;                            // максимальная длина

}

BlockingRs::~BlockingRs()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void BlockingRs::startRs(const QString& settings, COMMTIMEOUTS tm)
{
    QMutexLocker locker(&mutex);
    this->tm = tm;
    parse(settings);
    if (!isRunning())
        start();
}

void BlockingRs::run()
{
    QSerialPort serial;

    // устанавливаем параметры порта
    serial.setPortName(_name);
    serial.setBaudRate(baudRate);
    serial.setDataBits(dataBits);
    serial.setParity  (parity);
    serial.setStopBits(stopBits);
    if (!serial.open(QIODevice::ReadWrite))
    {
        emit (error(UNAVAILABLE));
        return;
    }

    mainLoop(serial);
}

// основной цикл приема/передачи данных
// эту функцию следует переопределять в производных классаъ для реализации логики и последовательности полудуплексного обмена
void BlockingRs::mainLoop(QSerialPort& serial)
{
    while (!quit)
    {
        // 1. Прием данных
        QByteArray data = readData(serial);
        // 2. разбор пакета
        doData(data);
        // 3. Передача ответв/запроса

        if (quit)
            break;
        QByteArray array("ACK!  ");
        serial.write(array);
    }
    if (serial.isOpen())
        serial.close();
}

void BlockingRs::exit()
{
    QMutexLocker locker(&mutex);
    quit = true;
}

// поиск маркера в байтовом массиве, при необходимости усекание массива вплоть до маркера
bool BlockingRs::FindMarker (QByteArray& data, BYTE marker)
{
    if (marker!=0)
    {
        while (data.count() > 0 && (BYTE)data[0] != marker)
        {
            data.remove(0,1);
        }
    }
    return data.count() > 0;
}

// виртуальная функция приема данных
// если определен маркер - принимает и накапливает данные начиная с маркера
// QSerialPort& serial  - порт,
// BYTE marker          - маркер начала пакета или 0
// int maxlength        - максимальный размер пакета
QByteArray BlockingRs::readData(QSerialPort& serial)
{
    QByteArray data;
    if (serial.waitForReadyRead(tm.ReadTotalTimeoutConstant))
    {
        data = serial.readAll();                            // прием первой пачки данных
        bool found = FindMarker (data, marker);             // усекаем до маркера, если задан

        // прием потока байт до тайм-аута, если еще не нашли маркер - ищем
        while (serial.waitForReadyRead(tm.ReadIntervalTimeout))
        {
            data += serial.readAll();
            if (marker>0 && !found)
                found = FindMarker (data, marker);          // усекаем до маркера, если задан
            if (data.count() >= maxlength)
            {
                emit (error(L_OVER));
                break;
            }
        }
    }
    else
        emit (timeout());
    return data;
}

void BlockingRs::doData(QByteArray& data)
{
    emit dataready(data);
}

// разбор строки типа "COM1,9600,N,8,1"
bool BlockingRs::parse(QString str)
{
    parsed=false;
    QStringList options = str.split(",");
    if (options.length() > 0)
    {
        parsed = true;
        _name   = options[0];
        if (options.length() > 0)
            baudRate = options[1].toInt(&parsed);
        if (options.length() > 1)
          parity =   options[2] == "E" ? QSerialPort::EvenParity : options[2] == "O" ? QSerialPort::OddParity : QSerialPort::NoParity;
        if (options.length() > 2)
            dataBits = (QSerialPort::DataBits)options[3].toInt(&parsed);
        if (options.length() > 3)
            stopBits = (QSerialPort::StopBits)options[4].toInt(&parsed);

    }
    return parsed;
}

// получить строку описания ошибки по коду
QString BlockingRs::errorText (DataError error)
{
    QString ret = "?";
    switch (error)
    {
        case TIMEOUT:     ret = "Нет данных в канале...";           break;
        case CRC:         ret = "Ошибка CRC";                       break;
        case L_OVER:      ret = "Слишком большая длина блока данных"; break;
        case FORMAT:      ret = "Нет данных в канале...";           break;
        case TRASH:       ret = "'Мусор' в канале...";              break;
        case UNAVAILABLE: ret = "Ошибка открытия порта";            break;
    }
    return ret;
}
