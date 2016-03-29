#include <QStringList>
#include <QDebug>
#include "rsbase.h"


RsBase::RsBase(QString settings)
{
    buf = QByteArray(2048, 0);
    indx_get = indx_put = 0;
    count = 0;
    overload = 0;

    name = "COM1";
    baudRate = 9600;
    parity   = QSerialPort::NoParity;
    dataBits = QSerialPort::Data8;
    stopBits = QSerialPort::OneStop;

    parse(settings);
}

RsBase::~RsBase()
{

}

// прием форматных данных   (определяется протоколом)
//bool RsBase::getData()
//{
//    return true;
//}
// чтение данных в соответствии с протоколом
//int RsReader::readData(class RsBase* rs)
//{
//    //rs->
//    return 0;
//}

// оформление пакета        (определяется протоколом)
QByteArray RsBase::prepareData(QByteArray data)
{
    return data;
}

// разбор строки типа "COM1,9600,N,8,1"
bool RsBase::parse(QString str)
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
    //setFlowControl(p.flowControl);

    // связываем сигналы и слоты
    connect(this, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(ReadData()));

    open(QIODevice::ReadWrite);
    QByteArray array("12345");
    this->write(array);

    return parsed;
}

void RsBase::ReadData()
{
    char ch;
    while (read(&ch, 1))
    {
        qDebug() << ch;
        mutex.lock();
        if (count > 0 && indx_get==indx_put)
        {
            overload++;
            qDebug() << "Переполнение входного буфера, всего " << overload << " байт игнорировано";
        }
        else
        {
            buf[indx_put++] = ch;
            if (indx_put >= buf.size())
                indx_put = 0;
            count++;
            allread++;
            water.wakeAll();
        }
        mutex.unlock();
    }
}

void RsBase::handleError(QSerialPort::SerialPortError error)
{
    if (error)
        qDebug() << "Ошибка открытия порта " << name << ": " << error;
}

// получить очередной байт, ожидая не более ms миллисекуд
char RsBase::GetChar(int ms)
{
    char ch;
    // если нет готовых данных - ждем ms миллисекунд
    if (!count)
    {
        watermutex.lock();
        bool ret = water.wait(&watermutex, ms);
        watermutex.unlock();
        if (!ret)
            return -1;                                      // нет данных в течение ms миллисекунд
    }

    // данные есть, выбираем
    mutex.lock();
    ch = buf[indx_get++];
    if (indx_get >= buf.size())
        indx_get = 0;
    count--;
    mutex.unlock();
    return ch;
}
