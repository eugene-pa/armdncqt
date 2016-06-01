#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x55aa55aa;                 // заголовок
const quint16 RemoteRq::paServerVersion = 1;                       // версия paServer

QHostAddress RemoteRq::localaddress;
QHostAddress RemoteRq::remoteaddress;

RemoteRq::RemoteRq()
{
    rq = rqEmpty;
    src = localaddress;
    dst = remoteaddress;
    remotePath = "";
}

RemoteRq::~RemoteRq()
{

}

void RemoteRq::SerializeBase(QDataStream &stream)
{
    stream << RemoteRq::streamHeader;
    stream << RemoteRq::paServerVersion;
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
    quint32 header;                                         // заголовок
    quint16 version;                                        // версия paServer
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


