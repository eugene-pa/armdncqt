#include "rsasinc.h"

// Интерфейсный класс для работы с COM-портом в стиле, принятом в ПО ДЦ ЮГ
//   а именно: с использованием перегруженной функции GetCh(int ms) - получить символ в течении ограниченного времени
//   Класс необходим по той причине, что класс QSerialPort, входящий в состав QT, не позволяет выбирать
//   данные посимвольно в аналогичной манере: при уведомлении о наличии данных еужно считать ВСЕ имеющиеся символы
// Интерфейсный класс выполняет следующие функции:
//   - открытие порта с заданными настройками в следующем формате (пример):  "COM1,9600,N,8,1"
//   - буферирование принимаемых данных в очереди FIFO и выдачу данных по требованию посимвольно (GetCh())
//   - передачу данных
// Важно:
// 1.Попытка реализовать прием данных в отдельном потоке std::thread без использования сигналов/слотов QT,
//   но путем использования waitForReadyRead, приводила к задержкам в обработке двнных:
//   данные не терялись, а зависали где-то в недрах QSerialPort и выдавались при приеме следующих данных
//   Возможно, можно было попробовать использовать QThread взамен std::thread (см.класс RxProxy)
// 2.В консольных приложениях есть проблемы использования QSerialPort: передачу данных надо завершать
//   вызовом waitForBytesWritten, иначе данные просто не передаются.
//   Причем документация утверждает, что этот вызов может приводить к зависанию GUI приложений
//   Поэтому рекомендуется раскомментировать определение CONSOLAPP в файле rsasinc.h для консольных приложений
// 3. Для синхронизации используются средства STL:
//      - std::mutex mtxBuf                 - блокировка доступа к очереди
//      - std::mutex mtxWater               - мьютекс для организации ожидания поступления данных
//      - std::condition_variable water     - условие ожидание приема данных, позволяющее организовать ожидание
//                                            ввода с пробуждением при приеме
// 4. В качестве контейнера используется стандартный контейнер std::deque<unsigned char> (хотя можно было использовать queue)
// 5. При использовании класса в консольном приложении необходимо выполнить ряд доп.условий:
//    - класс должен быть объявлен в основном потоке (так как в этом потоке будет цикл обработки сообщений)
//    - обязательно должен быть выполнен вызов QCoreApplication.exec(), иначе не работают сигналы и слоты
// 6. Для чтения используются 3 функции:
//      int  GetCh();             получить символ с ожиданием не более timeWaiting миллисекунд, иначе возврат -1
//      unsigned char GetChEx();  получить символ с ожиданием не более timeWaiting миллисекунд, иначе исключение RsException
//      int  GetCh(int ms);       получить символ с ожиданием не более ms миллисекунд, иначе возврат -1


// конструктор принимает параметры COM-порта, например: "COM1,9600,N,8,1"
RsAsinc::RsAsinc(QString  settings)
{
    this->settings = settings;
    rqExit = false;
    maxSize = 4096;
    timeWaiting = 100;
    parse(settings);
}

// разбор строки типа "COM1,9600,N,8,1"
bool RsAsinc::parse(QString str)
{
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
    serial.setPortName(name);
    serial.setBaudRate(baudRate);
    serial.setDataBits(dataBits);
    serial.setParity  (parity);
    serial.setStopBits(stopBits);
    //serial.setFlowControl(QSerialPort::HardwareControl);

    qDebug() << "Open port";
    if (!serial.open(QIODevice::ReadWrite))
        qDebug() << tr("Error open port");
    else
    {
        // подключаем сигнал QSerialPort::readyRead к своему обработчику RsAsinc::readData
        connect (&serial, SIGNAL(readyRead()), this, SLOT(readData()));

        // подключаем сигнал QSerialPort::SerialPortError к своему обработчику RsAsinc::handleError
        connect (&serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    }
    return parsed;
}

// проверка несущей
bool RsAsinc::CourierDetect()
{
    return serial.pinoutSignals() & QSerialPort::DataCarrierDetectSignal;
}

// слот обработки уведомления о наличии входных данных
void RsAsinc::readData()
{
    QByteArray data = serial.readAll();                 // читаем все
    mtxBuf.lock();                                      // блокируем очередь
    foreach (unsigned char bt, data)
    {
        if (buffer.size() >= maxSize)                   // если размер очереди превышает ограничение -
            buffer.pop_back();                          //      сдвигаем с потерей самого старого байта
                                                        // такое поведение - предмет обсуждения
        buffer.push_front((unsigned char)bt);           // запоминаем принятые данные
        // qDebug() << (char)bt;
    }
    mtxBuf.unlock();                                    // разблокируем очередь

    water.notify_all();
}

// слот обработки уведомления об ошибке COM-порта
void RsAsinc::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        //QMessageBox::critical(nullptr, tr("Critical Error"), serial.errorString());
        qDebug() << "Critical error: " << serial.errorString() << error;
        if (serial.isOpen())
            serial.close();
    }
    else
        qDebug() << "Eroor COM-port: " << serial.errorString() << error;
}

// получить сисвол с ожидаием не более timeWaiting миллисекунд
int RsAsinc::GetCh ()
{
    return GetCh (timeWaiting);
}

// получить символ с ожиданием не более timeWaiting миллисекунд
unsigned char RsAsinc::GetChEx()
{
    int ch;
    if ((ch = GetCh (timeWaiting))==-1)
        throw RsException();
    return (unsigned char)ch;
}


// получить сисвол с ожидаием не более ms миллисекунд
// если нет данных - возвращаем -1
int RsAsinc::GetCh (int ms)
{
    if (ms==0)
        ms = timeWaiting;
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


// передача массива данных по указателю заданной длины
bool RsAsinc::Send (void *p, int length)
{
#ifdef CONSOLAPP
    if (serial.write((const char *)p, length) <= 0)
        return false;
    return serial.waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return length == serial.write(const char *p, length);
#endif
}

// передача массива данных QByteArray
bool RsAsinc::Send (QByteArray& data)
{
#ifdef CONSOLAPP
    if (serial.write (data) <= 0)
        return false;
    return serial.waitForBytesWritten(-1);         // только для консольных приложений!
#else
    return data.length() == serial.write (data);
#endif
}

// завершение работы
void RsAsinc::Close()
{
    if (serial.isOpen())
        serial.close();
}

