#include "blockingrs.h"

// Конструктор
// QObject *parent      - родитель
// BYTE marker          - маркер начала пакета или 0
// int maxlength        - максимальный размер пакета
BlockingRs::BlockingRs(QObject *parent, BYTE marker, int maxlength) : QThread(parent)
{
    // таймауты по умолчанию
    tm = { 100,                                             // максимальный межбайтовый интервал
           0,
           400,                                             // максимальное время ожидания начала приема данных
           1, 500 };
    quit = false;

    this->parent = parent;                                  // родитель
    this->marker = marker;                                  // маркер или 0
    this->maxlength = maxlength;                            // максимальная длина
    pSerial = nullptr;
}

BlockingRs::~BlockingRs()
{
    mutex.lock();
    quit = true;
    mutex.unlock();
    qDebug() << "~BlockingRs() wait";
    wait();
    qDebug() << "end ~BlockingRs()";
}

// запуск рабочего потока приема/передачи
// QString& settings - строка типа: "COM1,38400,N,8,1"
// - порт
// - скорость
// - четность
// - бит данных
// - стоп-бит
void BlockingRs::startRs(const QString& settings, COMMTIMEOUTS tm)
{
    QMutexLocker locker(&mutex);
    this->tm = tm;
    parse(settings);
    if (!isRunning())
        start();
}

// рабочая функция потока
void BlockingRs::run()
{
    qDebug() << "Start BlockingRs::run() thread";
    QSerialPort serial;                                     // создаем порт на стеке рабочей функции
    pSerial = &serial;

    serial.setPortName(_name);                              // устанавливаем параметры порта
    serial.setBaudRate(baudRate);
    serial.setDataBits(dataBits);
    serial.setParity  (parity);
    serial.setStopBits(stopBits);
    if (!serial.open(QIODevice::ReadWrite))                 // открываем порт
    {
        qDebug() << (lastErrorText = serial.errorString());
        //std::wcout << qToStdWString(lastErrorText = serial.errorString());
        emit (error(UNAVAILABLE));
        return;
    }

    mainLoop();                                             // вызов вирт.функции реализации протокола приема/передачи вплоть до завешения

    if (serial.isOpen())
        serial.close();
    pSerial = nullptr;
    qDebug() << "End BlockingRs::run() thread";
    //emit(finished());
}

// основной цикл приема/передачи данных
// эту функцию следует переопределять в производных классах для реализации логики и последовательности полудуплексного обмена
void BlockingRs::mainLoop()
{
    while (!quit)
    {
        // 1. Прием данных
        QByteArray data = readData(*pSerial);
        if (quit)   break;

        // 2. разбор пакета
        doData(data);

        // тайм-ауты, например:
        wait(500);                                          // или msleep(500);
        if (quit)   break;

        // 3. Передача ответв/запроса
        QByteArray array("ACK!  ");
        pSerial->write(array);
    }
}

void BlockingRs::exit()
{
    QMutexLocker locker(&mutex);
    quit = true;
    //QThread::exit(0);
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
                qDebug() << L"Overhead!";
                //std::wcout << L"Переполнение!" << endl;
                emit (error(L_OVER));
                break;
            }
        }
    }
    else
    {
        qDebug() << "Timeout!";
        //std::wcout << L"Timeout!" << endl;
        emit (timeout());
    }
    if (data.length())
        qDebug() << "Data length: " << data.length();
        //std::wcout << L"Длина принятых данных = " << data.length() << endl;
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
        case TIMEOUT:     ret = "Нет данных в канале...";             break;
        case CRC:         ret = "Ошибка CRC";                         break;
        case L_OVER:      ret = "Слишком большая длина блока данных"; break;
        case FORMAT:      ret = "Нет данных в канале...";             break;
        case TRASH:       ret = "'Мусор' в канале...";                break;
        case UNAVAILABLE: ret = "Ошибка открытия порта";              break;
    }
    return ret;
}

QString BlockingRs::errorText (int error)
{
    return errorText((DataError)error);
}
