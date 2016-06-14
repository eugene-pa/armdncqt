#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x48454144;                 // заголовок HEAD
const quint16 RemoteRq::paServerVersion = 1;                       // версия paServer

QHostAddress RemoteRq::localaddress("127.0.0.1");
QHostAddress RemoteRq::remoteaddress("127.0.0.1");

RemoteRq::RemoteRq()
{
    rq = rqEmpty;
    header  = RemoteRq::streamHeader;
    version = paServerVersion;

    src = localaddress;
    dst = remoteaddress;

    remotePath = "";
}

RemoteRq::RemoteRq(RemoteRqType req, QString& fullpath)
{
    rq = req;
    fullPath = fullpath;

    header  = RemoteRq::streamHeader;
    version = paServerVersion;

    src = localaddress;
    dst = remoteaddress;

    // разделяем удаленный путь
    QString host;
    ParseNestedIp(fullPath, host, remotePath);
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
    stream << fullPath;
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
    stream >> fullPath;
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


QString RemoteRq::toString()
{
    switch(rq)
    {
        case rqEmpty       : return "rqEmpty"                                                                                  ; break;
        case rqAbout       : return "Запрос информации о версии сервиса и хосте"                                               ; break;
        case rqDirs        : return "Запрос списка каталогов"                                                                  ; break;
        case rqFileInfo    : return "Запрос информации о файле"                                                                ; break;
        case rqFilesInfo   : return "Запрос информации о всех файлах каталога"                                                 ; break;
        case rqFilesSize   : return "Запрос размера заданных файлов"                                                           ; break;
        case rqDrives      : return "Запрос списка логических устройств"                                                       ; break;
        case rqProcesses   : return "Запрос списка процессов"                                                                  ; break;
        case rqProcesseInfo: return "Запрос информации о процессе"                                                             ; break;
        case rqTempFile    : return "Запрос временной копии файла для копирования"                                             ; break;
        case rqTempFilesZip: return "Запрос архивирования указанных файлов во временом файле"                                  ; break;
        case rqTempDirZip  : return "Запрос архивирования каталога во временом файле"                                          ; break;
        case rqDeleteTemp  : return "Запрос удаления всех временных файлов"                                                    ; break;
        case rqRead        : return "Запрос чтения блока данных файла"                                                         ; break;
        default: return "???";
    }
}

// разбор вложенного пути, например, путь 192.168.0.100:8080/10.52.19.31:8080/192.168.1.11:8080 разбивается на 2 лексемы:
// - 192.168.0.100:8080
// - 10.52.19.31:8080/192.168.1.11:8080
bool RemoteRq::ParseNestedIp(QString& ipportpath, QString& root, QString& path)
{
    bool ret = true;
    root = ipportpath;
    path.clear();
    QRegularExpressionMatch match = QRegularExpression("\\A([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}/\\b").match(ipportpath);

    if (match.hasMatch())
    {
        int rootlength = match.captured().length();
        root = match.captured().left(rootlength-1);
        path = ipportpath.mid(rootlength);
    }
//    else
//        ret = false;
    return ret;
}
