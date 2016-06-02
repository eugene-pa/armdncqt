#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x55aa55aa;                 // заголовок
const quint16 RemoteRq::paServerVersion = 1;                       // версия paServer

QHostAddress RemoteRq::localaddress;
QHostAddress RemoteRq::remoteaddress;

RemoteRq::RemoteRq(RemoteRqType req)
{
    rq = req;

    header  = RemoteRq::streamHeader;
    version = paServerVersion;

    src = localaddress;
    dst = remoteaddress;
    remotePath = "";

}


RemoteRq::~RemoteRq()
{

}

QByteArray RemoteRq::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream stream (&buf);

    stream << header;
    stream << version;

    stream << (int)rq;
    stream << src;
    stream << dst;
    stream << remotePath;
    stream << param;
    stream << reserv1;
    stream << reserv2;
    stream << reserv3;

    return buf.buffer();
}

void RemoteRq::Deserialize (QDataStream &stream)
{
    stream >> header;
    stream >> version;
    int rqt; stream >> rqt;
    rq = (RemoteRqType)rqt;
    stream >> src;
    stream >> dst;
    stream >> remotePath;
    stream >> param;
    stream >> reserv1;
    stream >> reserv2;
    stream >> reserv3;
}

/*
void RemoteRq::SerializeBase(QDataStream &stream)
{
    stream << header;
    stream << version;

    stream << (int)rq;
    stream << src;
    stream << dst;
    stream << remotePath;
    stream << param;
    stream << reserv1;
    stream << reserv2;
    stream << reserv3;
}

void RemoteRq::DeserializeBase(QDataStream &stream)
{
    stream >> header;
    stream >> version;
    int rqt; stream >> rqt;
    rq = (RemoteRqType)rqt;
    stream >> src;
    stream >> dst;
    stream >> remotePath;
    stream >> param;
    stream >> reserv1;
    stream >> reserv2;
    stream >> reserv3;
}
*/

