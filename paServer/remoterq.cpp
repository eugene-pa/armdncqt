#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x48454144;                 // заголовок HEAD
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
    stream << param2;
    stream << param3;
    stream << param4;
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
    stream >> param2;
    stream >> param3;
    stream >> param4;
    stream >> reserv1;
    stream >> reserv2;
    stream >> reserv3;
}


// получить имя запроса
QString RemoteRq::getRqName(RemoteRqType type)
{
    QString ret = "Не идентифицирован запрос #" + QString::number((int)type);
    switch (type)
    {
    case rqAbout       : ret = "rqAbout";  break;
    case rqDirs        : ret = "rqDirs"; break;
    case rqFileInfo    : ret = "rqFileInfo"; break;
    case rqFilesInfo   : ret = "rqFilesInfo"; break;
    case rqFilesSize   : ret = "rqFilesSize"; break;
    case rqDrives      : ret = "rqDrives"; break;
    case rqProcesses   : ret = "rqProcesses"; break;
    case rqProcesseInfo: ret = "rqProcesseInfo"; break;
    case rqTempFile    : ret = "rqTempFile"; break;
    case rqTempFilesZip: ret = "rqTempFilesZip"; break;
    case rqTempDirZip  : ret = "rqTempDirZip"; break;
    case rqDeleteTemp  : ret = "rqDeleteTemp"; break;
    case rqRead        : ret = "rqRead"; break;
    case rqEnd         : ret = "rqEnd"; break;
    default:                            break;
    }
    return ret;
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

