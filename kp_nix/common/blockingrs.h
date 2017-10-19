#ifndef BLOCKINGRS_H
#define BLOCKINGRS_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSerialPort>

#include <mutex>                                            // std::mutex
#include <queue>											// std::queue
#include <condition_variable>                               // std::condition_variable

#define CONSOLAPP

class BlockingRS : public QThread
{
    Q_OBJECT
public:
    explicit BlockingRS(QString config, QObject *parent = nullptr);
    ~BlockingRS();

    void run() Q_DECL_OVERRIDE;

    int  GetCh();                                           // получить символ с ожиданием не более timeWaiting миллисекунд, иначе возврат -1
    unsigned char GetChEx();                                // получить символ с ожиданием не более timeWaiting миллисекунд, иначе исключение RsException
    int  GetCh(int ms);                                     // получить символ с ожиданием не более ms миллисекунд, иначе возврат -1
    bool Send (void *, int length);                         // передача массива char заданной длины
    bool Send (QByteArray& data);                           // передача массива QByteArray
    void SetTimeWaiting(int ms) { timeWaiting = ms; }       // изменение времени ожидания по умолчанию
    //QSerialPort& Serial() { return serial; }                // доступ непосредственно к классу QSerialPort
    bool Parsed() { return parsed; }                        // проверка успешности парсинга параметров порта
    bool CourierDetect();                                   // проверка несущей
    bool IsOpen() { return serial!=nullptr && serial->isOpen(); } // проверка 'порт открыт'?
    void Close();                                           // завершение работы

private:
    QSerialPort * serial;                                   // экземпляр QSerialPort

    std::deque<unsigned char> bufIn;                        // очередь FIFO принятых     данных
    std::mutex mtxBufIn;                                    // блокировка доступа к входной очереди

    std::deque<unsigned char> bufOut;                       // очередь FIFO передаваемых данных
    std::mutex mtxBufOut;                                   // блокировка доступа к выходной очереди
    bool errorSend;                                         // ошибка при передаче

    std::mutex mtxWater;                                    // мьютекс для организации ожидания поступления данных
    std::condition_variable water;                          // условие ожидание приема данных

    int timeWaiting;                                        // время ожидания символа по умолчанию, мс

    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString name;                                           // имя порта

    std::size_t maxSize;                                    // максимально допустимая длина невыбранных данных,
                                                            // после которой при приеме удаляются самые старые данные
    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"
    bool parsed;                                            // true - описание порта корректное
    bool rqExit;                                            // завершение работы
};

class RsException
{
public:
    enum Error
    {
        timeout = 1,
    };
    RsException() { error = timeout; }
private:
    Error error;
};

#endif // BLOCKINGRS_H
