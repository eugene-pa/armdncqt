#include "blockingrs.h"

BlockingRs::BlockingRs(QObject *parent) : QThread(parent)
{
    tm = { 100,0, 400, 1, 500 };
    quit = false;
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

        emit error(tr("Can't open %1, error code %2").arg(_name).arg(serial.error()));
        return;
    }
    while (!quit)
    {
        QByteArray data = readData(serial);
        // разбор пакета
        doData(data);

        QByteArray array("1111--2222");
        serial.write(array);
        int a = 99;
    }
}

QByteArray BlockingRs::readData(QSerialPort& serial)
{
    QByteArray data;
    if (serial.waitForReadyRead(tm.ReadTotalTimeoutConstant))
    {
        data += serial.readAll();
        while (serial.waitForReadyRead(tm.ReadIntervalTimeout))
            data += serial.readAll();

    }
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

    //setFlowControl(p.flowControl);

    // связываем сигналы и слоты
//    connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
//    connect(this, SIGNAL(readyRead()), this, SLOT(ReadData()));

//    open(QIODevice::ReadWrite);

    return parsed;
}