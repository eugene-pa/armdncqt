#ifndef RSBASE_H
#define RSBASE_H

#include <QByteArray>


class RsReader : public QObject
{
    Q_OBJECT
public slots:
    virtual int readData(class RsBase*);

signals:
    void resultReady(const QByteArray);
};


class RsBase : public QSerialPort
{
    struct Settings
    {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
public:
    RsBase(Settings settings);
    ~RsBase();

    virtual bool getData();                                 // прием форматных данных   (определяется протоколом)
    virtual QDyteAttay prepareData(QByteArray);             // оформление пакета        (определяется протоколом)
    bool send (QByteArray);                                 // передача данных пачкой

signals:
    void DataReady(QByteArray);
    void Error(int error);


private:
    Settings settings;                                      // настройки

};

#endif // RSBASE_H