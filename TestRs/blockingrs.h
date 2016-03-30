#ifndef BLOCKINGRS_H
#define BLOCKINGRS_H

#include <QSerialPort>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "../common/defines.h"

class BlockingRs : public QThread
{
    Q_OBJECT
public:
    BlockingRs(QObject *parent = 0);
    ~BlockingRs();

    void startRs(const QString &settings, COMMTIMEOUTS tm);
    void run();

signals:
    void dataready(QByteArray);
    void error    (const QString &s);
    void timeout  (const QString &s);

private:
    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"
    virtual QByteArray readData(QSerialPort& serial);       // можно перекрыть
    virtual void doData(QByteArray&);                       // можно перекрыть

    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString _name;                                           // имя порта
    qint32 baudRate;                                        // скорость
    QSerialPort::Parity parity;                             // четность
    QSerialPort::DataBits dataBits;                         // бит данных
    QSerialPort::StopBits stopBits;                         // число стоп-бит

    QMutex mutex;
    bool quit;
    COMMTIMEOUTS tm;
    bool parsed;
};

#endif // BLOCKINGRS_H
