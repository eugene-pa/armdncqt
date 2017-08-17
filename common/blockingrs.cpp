#include <mutex>                                            // std::mutex
#include "blockingrs.h"

void Log(std::wstring);

BlockingRS::BlockingRS(QString config, QObject *parent)
{
    // следует помнить, что код, выполняемый в конструкторе, выполняется в вызывающем потоке
    settings = config;
    serial = nullptr;
    rqExit = false;
    maxSize = 4096;
    timeWaiting = 100;
}

BlockingRS:: ~BlockingRS()
{
    rqExit = true;
}

// поток работы с COM-портом.
// здесь д.б. открытие, приема и передача
// для этого функция SEND дожна просто формировать запрос, а передача должна производиться тут же!
void BlockingRS::run()
{
    parse(settings);
    Log(L"Старт потока BlockingRS. Thread #" + (QString::number((int)currentThreadId())).toStdWString() + L". " + settings.toStdWString());

    while (!rqExit)
    {
        if (serial->waitForReadyRead(timeWaiting))
        {
            QByteArray data = serial->readAll();                 // получаем все
            mtxBuf.lock();                                      // блокируем очередь
            foreach (unsigned char bt, data)
            {
                if (buffer.size() >= maxSize)                   // если размер очереди превышает ограничение -
                    buffer.pop_back();                          //      сдвигаем с потерей самого старого байта
                                                                // такое поведение - предмет обсуждения
                buffer.push_front((unsigned char)bt);           // запоминаем принятые данные
            }
            mtxBuf.unlock();                                    // разблокируем очередь
        }
    }
    Log(L"Завершение потока BlockingRS " + name.toStdWString());
}

// разбор строки типа "COM1,9600,N,8,1"
bool BlockingRS::parse(QString str)
{
    serial = new QSerialPort();
    qint32                  baudRate = 9600;                    // скорость
    QSerialPort::Parity     parity   = QSerialPort::NoParity;   // четность
    QSerialPort::DataBits   dataBits = QSerialPort::Data8;      // бит данных
    QSerialPort::StopBits   stopBits = QSerialPort::OneStop;    // число стоп-бит

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
    serial->setPortName(name);
    serial->setBaudRate(baudRate);
    serial->setDataBits(dataBits);
    serial->setParity  (parity);
    serial->setStopBits(stopBits);
    //serial.setFlowControl(QSerialPort::HardwareControl);

    Log(L"Open port");
    if (!serial->open(QIODevice::ReadWrite))
        Log(L"Error open port");

    return parsed;
}

// получить сисвол с ожидаием не более timeWaiting миллисекунд
int BlockingRS::GetCh ()
{
    return GetCh (timeWaiting);
}

// получить символ с ожиданием не более timeWaiting миллисекунд
unsigned char BlockingRS::GetChEx()
{
    int ch;
    if ((ch = GetCh (timeWaiting))==-1)
        throw RsException();
    return (unsigned char)ch;
}


// получить сисвол с ожидаием не более ms миллисекунд
// если нет данных - возвращаем -1
int BlockingRS::GetCh (int ms)
{
    int ch = -1;
    if (ms==0)
        ms = timeWaiting;
    mtxBuf.lock();
    bool ready = buffer.size();
    mtxBuf.unlock();
    if (!ready)
    {
        std::unique_lock<std::mutex> lck(mtxWater);
        std::cv_status ret = water.wait_for(lck, std::chrono::milliseconds(ms));
        if (ret ==  std::cv_status::timeout)
            return ch;
    }

    mtxBuf.lock();
    if (buffer.size() >0)
    {
        ch = buffer.back();
        buffer.pop_back();
    }
    mtxBuf.unlock();
    return ch;
}


// передача массива данных по указателю заданной длины
bool BlockingRS::Send (void *p, int length)
{
#ifdef CONSOLAPP
    //QByteArray data((const char *)p, length);
    //length = serial.write(data);
    //const char * ptr = (const char *)p;
    //length = serial.write(ptr, (qint64)length);
    if (serial->write((const char *)p, (qint64)length) <= 0)
        return false;
    return serial->waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return length == serial->write((const char *)p, length);
#endif
}

// передача массива данных QByteArray
bool BlockingRS::Send (QByteArray& data)
{
#ifdef CONSOLAPP
    if (serial->write (data) <= 0)
        return false;
    return serial->waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return data.length() == serial->write (data);
#endif
}

