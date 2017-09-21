#ifndef RSBASE_H
#define RSBASE_H

#include <QObject>
#include <QByteArray>
#include <QSerialPort>
#include <QMutex>
#include <QWaitCondition>


class RsBase : public QSerialPort
{
    Q_OBJECT
public:
    RsBase(QString settings);
    ~RsBase();

    bool send (QByteArray);                                 // передача данных пачкой
    char GetChar(int ms);                                   // получить очередной байт, ожидая не более ms миллисекуд
    QString name() { return _name; }

signals:
    void DataReady(QByteArray);
    void Error(int error);

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void ReadData();

private:
    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString _name;                                          // имя порта
    qint32 baudRate;                                        // скорость
    QSerialPort::Parity parity;                             // четность
    QSerialPort::DataBits dataBits;                         // бит данных
    QSerialPort::StopBits stopBits;                         // число стоп-бит

    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"
    bool parsed;                                            // опсание порта корректно

    QByteArray buf;                                         // кольцевой буфер данных
    int indx_get;                                           // смещение для чтения данных
    int indx_put;                                           // смещение для записи данных
    int count;                                              // число считанных байт данных
    QMutex mutex;                                           // блокировка очереди
    QWaitCondition water;                                   // условие ожидания данных
    QMutex watermutex;                                      // блокировка условия ожидания
    uint overload;                                          // счетчик переполнения
    uint allread;                                           // всего принято за время работы
};

#endif // RSBASE_H
