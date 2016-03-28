#ifndef RSBASE_H
#define RSBASE_H

#include <QByteArray>

class RsBase : public QSerialPort
{
public:
    RsBase();
    ~RsBase();

    bool send(QByteArray);
signals:
    void DataReady(QByteArray);
    void Error(int error);


};

#endif // RSBASE_H
