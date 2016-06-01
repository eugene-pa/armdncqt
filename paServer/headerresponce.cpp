#include "headerresponce.h"

HeaderResponce::HeaderResponce()
{

}

HeaderResponce::~HeaderResponce()
{

}

void HeaderResponce::Serialize  (QDataStream &stream)
{
    stream << RemoteRq::streamHeader;
    stream << RemoteRq::paServerVersion;
    stream << (int)rq;
    stream << src;
    stream << dst;
    stream << fullPath;
    stream << reserv1;
    stream << reserv2;
    stream << reserv3;
}

void HeaderResponce::Deserialize(QDataStream &stream)
{
    quint32 header;                                         // заголовок
    quint16 version;                                        // версия paServer
    stream >> header;
    stream >> version;

    int rqt; stream >> rqt;
    rq = (RemoteRqType)rqt;

    stream >> src;
    stream >> dst;
    stream >> fullPath;
    stream >> reserv1;
    stream >> reserv2;
    stream >> reserv3;
}
