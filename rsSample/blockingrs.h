#ifndef BLOCKINGRS_H
#define BLOCKINGRS_H

#include <QSerialPort>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "defines.h"

// Класс BlockingRs реализует блокирующие методы работы с асинхронным последовательным интерфейсом
// Класс может быть унаследован от

class BlockingRs : public QThread
{
    Q_OBJECT
public:
    BlockingRs(QObject *parent = 0, BYTE marker=0, int maxlength=4048);
    ~BlockingRs();

    enum DataError
    {
        TIMEOUT     = 0x81,                                 // тайм аут
        CRC         = 0x82,                                 // несовпадение CRC
        L_OVER      = 0x83,                                 // превышен размер
        FORMAT      = 0x84,                                 // нарушен формат
        TRASH       = 0x85,                                 // "мусор" в канале
        UNAVAILABLE = 0x86,                                 // ошибка открытия порта
    };

    void startRs(const QString &settings, COMMTIMEOUTS tm);
    void run();
    QString& name() { return _name; }
    static QString errorText (DataError error);

    // виртуальные функции
    virtual void mainLoop();                                // основной цикл
    virtual QByteArray readData(QSerialPort& serial);       // прием пакета данных с заданными таймаутами, маркером и максимальной длиной
    virtual void doData(QByteArray&);                       // можно перекрыть

private slots:
    void exit();

signals:
    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void error    (int);                                    // сигнал-уведомление об ошибке
    void timeout  ();                                       // сигнал-уведомление о таймауте

protected:
    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"

    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString _name;                                          // имя порта
    qint32 baudRate;                                        // скорость
    QSerialPort::Parity parity;                             // четность
    QSerialPort::DataBits dataBits;                         // бит данных
    QSerialPort::StopBits stopBits;                         // число стоп-бит

    QSerialPort * pSerial;
    QMutex mutex;
    bool quit;
    COMMTIMEOUTS tm;
    bool parsed;

    QObject * parent;                                       // родитель
    BYTE marker;                                            // маркер или 0
    int maxlength;                                          // максимальная длина
//    QSerialPort serial;

    bool FindMarker (QByteArray&, BYTE marker=0);
};

#endif // BLOCKINGRS_H
