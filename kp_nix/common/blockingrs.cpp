#include <mutex>                                            // std::mutex
#include "blockingrs.h"

// Блокирующий класс работы с COM-портом на основе QSerialPort
// Причины создания класса с такой архитектурой:
// QSerialPort не позволяет корректно работать с одним экземпляром из разных потоков
// В частности: принимать данные в одном потоке, а выдавать в другом.
// Более того, создание, прием и выдача должны выполняться в одном потоке
// Поэтому реализация протоколов взаимодействия по COM-порту с разными системами ЖАТ
// в отдельных потоках в стиле ПО ДЦ ЮГ в асинхронном режиме была проблематичной:
// для работы сигналов и слотов класс нужно создавать в потоке с loop event, например GUI,
// соответственно и корректная передача должна осуществляться из этого класса.
// Передача данных непосредственно из потока работала, но приводила к
// выводу диагностических сообщений: Invalid parameter passed to C runtime function.
// Настоящая реализация предполагает буферирование входных и выходных данных.
// Интерфейс с родителем - через функции Get и Send
// Непосредственный прием и выдача данных осуществляются в цикле функции BlockingRS::run()
//
// TODO: доработать механизм уведомлений
//

void Log(std::wstring);                                                 // функция лога объявляется в вызывающей программе

BlockingRS::BlockingRS(QString config, QObject *parent)
{
    Q_UNUSED(parent)

    // следует помнить, что код, выполняемый в конструкторе, выполняется в вызывающем потоке
    settings = config;
    serial = nullptr;
    rqExit = false;
    maxSize = 4096;
    timeWaiting = 10;
}

BlockingRS:: ~BlockingRS()
{
    rqExit = true;
}


// поток работы с COM-портом должен включать функции открытия, приема и передачи
// в рабочем цикле выполняем:
// - ожидание в течении не более 10 мсек приема данных, прием и наковление в bufIn
// - отправляем все, что есть в исходящем буфере bufOut
void BlockingRS::run()
{
    Log(L"Старт потока BlockingRS. Thread #" + (QString::number((int)currentThreadId())).toStdWString() + L". " + settings.toStdWString());
    parse(settings);

    while (serial->isOpen() && !rqExit)
    {
        if (serial->waitForReadyRead(10))
        {
            QByteArray data = serial->readAll();                // получаем все
            mtxBufIn.lock();                                    // блокируем очередь
            foreach (unsigned char bt, data)
            {
                if (bufIn.size() >= maxSize)                    // если размер очереди превышает ограничение -
                    bufIn.pop_front();                          //      сдвигаем с потерей самого старого байта
                                                                // такое поведение - предмет обсуждения
                bufIn.push_back((unsigned char)bt);             // запоминаем принятые данные
            }
            mtxBufIn.unlock();                                  // разблокируем очередь
        }

        // передача из буфера
        QByteArray toSend;
        mtxBufOut.lock();
        while (bufOut.size() > 0)
        {
            toSend.append((char)bufOut.front());
            bufOut.pop_front();
        }
        mtxBufOut.unlock();
        if (toSend.size() > 0)
        {
            // нужно генерить сигнал ТАЙМАУТ ПРИ ВЫВОДЕ
            errorSend = serial->write (toSend) > 0;
            serial->waitForBytesWritten();                    // только для консольных приложений!
        }
    }
    Log(L"Закрываем порт  " + name.toStdWString());
    serial->close();
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

    Log(L"Открываем порт " + name.toStdWString());
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
    mtxBufIn.lock();
    bool ready = bufIn.size();
    mtxBufIn.unlock();
    if (!ready)
    {
        std::unique_lock<std::mutex> lck(mtxWater);
        std::cv_status ret = water.wait_for(lck, std::chrono::milliseconds(ms));
        if (ret ==  std::cv_status::timeout)
            return ch;
    }

    mtxBufIn.lock();
    if (bufIn.size() >0)
    {
        ch = bufIn.front();
        bufIn.pop_front();
    }
    mtxBufIn.unlock();
    return ch;
}


// передача массива данных по указателю заданной длины
bool BlockingRS::Send (void *p, int length)
{
    mtxBufOut.lock();
    for (int i=0; i<length; i++)
        bufOut.push_back(((unsigned char *)p)[i]);
    bool ret = bufOut.size() < maxSize;
    mtxBufOut.unlock();
    return ret;
}

// передача массива данных QByteArray
bool BlockingRS::Send (QByteArray& data)
{
    mtxBufOut.lock();
    foreach (unsigned char bt, data)
        bufOut.push_back(bt);
    bool ret = bufOut.size() < maxSize;
    mtxBufOut.unlock();
    return ret;
}

// завершение работы
void BlockingRS::Close()
{
    Log(L"Запрос завершения работы на порту " + name.toStdWString());
    rqExit = true;
    QThread::msleep(500);
}

