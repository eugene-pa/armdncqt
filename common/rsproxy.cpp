#include <QDebug>
#include "rsproxy.h"

RsProxy::RsProxy(QString  settings)
{
    ScanDelay = 50;
    this->settings = settings;
    baudRate = 9600;
    parity   = QSerialPort::NoParity;
    dataBits = QSerialPort::Data8;
    stopBits = QSerialPort::OneStop;
    rqExit = false;
    maxSize = 4096;
    parse(settings);
}


// разбор строки типа "COM1,9600,N,8,1"
bool RsProxy::parse(QString str)
{
    parsed=false;
    QStringList options = str.split(",");
    if (options.length() > 0)
    {
        parsed = true;
        name   = options[0];
        if (options.length() > 0)
            baudRate = options[1].toInt(&parsed);
        if (options.length() > 1)
          parity =   options[2] == "E" ? QSerialPort::EvenParity : options[2] == "O" ? QSerialPort::OddParity : QSerialPort::NoParity;
        if (options.length() > 2)
            dataBits = (QSerialPort::DataBits)options[3].toInt(&parsed);
        if (options.length() > 3)
            stopBits = (QSerialPort::StopBits)options[4].toInt(&parsed);
    }

    // устанавливаем параметры порта
    setPortName(name);
    setBaudRate(baudRate);
    setDataBits(dataBits);
    setParity  (parity);
    setStopBits(stopBits);
    setFlowControl(QSerialPort::HardwareControl);

    qDebug() << "Open port";
    if (!open(QIODevice::ReadWrite))
        qDebug() << "Error open port";
    else
    {
        pThreadRead	= new std::thread ( ThreadReading , (long)this);	// поток чтения данных
    }

    return parsed;
}


// функция чтения данных
void RsProxy::ThreadReading (long param)
{
    RsProxy * p = (RsProxy *) param;
    qDebug() << "Start ThreadReading";
    while (!p->rqExit)
    {
        if (p->waitForReadyRead())
        {
            QByteArray data = p->readAll();
            p->mtxBuf.lock();
            foreach (unsigned char bt, data)
            {
                if (p->buffer.size() >= p->maxSize)
                    p->buffer.pop_back();
                p->buffer.push_front((unsigned char)bt);
                qDebug() << (char)bt;
            }
            p->mtxBuf.unlock();
            p->water.notify_all();
        }
    }
    qDebug() << "End ThreadReading";
}


void RsProxy::Close()
{
    rqExit = true;
}


// передача массива данных по указателю заданной длины
bool RsProxy::Send (const char *p, int length)
{
#ifdef CONSOLAPP
    if (write(p, length) <= 0)
        return false;
    return waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return length == write(p, length);
#endif
}

// передача массива данных QByteArray
bool RsProxy::Send (QByteArray& data)
{
#ifdef CONSOLAPP
    if (write (data) <= 0)
        return false;
    return waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return data.length() == write (data);
#endif
}

// получить сисвол с ожидаием не более ms миллисекунд
// если нет данных - возвражаем -1
int RsProxy::GetCh (int ms)
{
    mtxBuf.lock();
    bool ready = buffer.size();
    mtxBuf.unlock();
    if (!ready)
    {
        std::unique_lock<std::mutex> lck(mtxWater);
        if (water.wait_for(lck, std::chrono::milliseconds(ms)) ==  std::cv_status::timeout)
            return -1;
    }
    mtxBuf.lock();
    int ch = buffer.back();
    buffer.pop_back();
    mtxBuf.unlock();
    return ch;
}

// перегруженная функция; ret = true - прочитали.
unsigned char RsProxy::GetCh(bool& ret, int ms)
{
    ret = false;
    return 0;
}
