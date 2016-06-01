#ifndef RQABOUT_H
#define RQABOUT_H

#include <QCoreApplication>
#include <QTextCodec>
#include "remoterq.h"
#include "brieffileinfo.h"

class RqAbout : public RemoteRq
{
public:
    RqAbout();
    ~RqAbout() { }

    QByteArray Serialize();
    void Deserialize(QByteArray& data);
protected:
};

#endif // RQABOUT_H


