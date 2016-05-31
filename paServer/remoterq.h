#ifndef REMOTERQ_H
#define REMOTERQ_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>
#include <QBuffer>

// удаленный запрос

extern void log(QString&);                              // глобальная функция лога

enum RemoteRqType
{
    rqEmpty     = 0,
    rqAbout     = 1,                                    // информация о версии сервиса и хосте
//  rqDirInfo   = 2,                                    // информация о каталоге: дата создания, число файлов, число папок, размер
//  rqDirSize   = 3,                                    // размер каталога в байтах
    rqDirs      = 4,                                    // запрос списка каталогов
    rqFileInfo  = 5,                                    // запрос информации о файле, в том числе версии
    rqFilesInfo = 6,                                    // запрос информации о всех файлах каталога
    rqFilesSize = 7,                                    // запрос размера заданных файлов
    rqDrives    = 8,                                    // запрос списка логических устройств
    rqProcesses = 9,                                    // запрос списка процессов
    rqProcesseInfo = 10,                                // запрос информации о процессе
    rqTempFile     = 11,                                // запрос временной копии файла для копирования
    rqTempFilesZip = 12,                                // запрос архивирования указанных файлов во временом файле
    rqTempDirZip   = 13,                                // запрос архивирования каталога во временом файле
    rqDeleteTemp   = 14,                                // запрос удаления всех временных файлов
    rqRead         = 15,                                // запрос чтения части файла
    rqEnd          = 16,
};


class RemoteRq
{
public:

    RemoteRq();
    ~RemoteRq();

    static const quint32 streamHeader;                      // заголовок
    static const quint16 paServerVersion;                   // версия paServer

    virtual QByteArray data() { return QByteArray((const char*)this, sizeof(RemoteRq)); }
    virtual QByteArray prepare();                            // сформировать ответ

    bool isRemote() { return remotePath.length() == 0; }

protected:
    RemoteRqType rq;                                        // запрос
    QHostAddress src;                                       // IP источника запроса
    QHostAddress dst;                                       // IP назначение запроса
    QString      remotePath;                                // удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост  tcp://10.52.19.71/tcp://192.168.1.1
    QString      reserv1;                                   // резерв
    int          reserv2;                                   // резерв
};

class BriefFileInfo
{
public:
    BriefFileInfo() { }
    BriefFileInfo (QFileInfo& fi);                          // конструктор
    void fill(QFileInfo& fi);
//    void Serialize  (QDataStream& stream);
//    void Deserialize(QDataStream& stream);

    friend QDataStream &operator <<(QDataStream &stream, BriefFileInfo info)
    {
        stream << info._name;
        stream << info._lastChanged;
        stream << info._created;
        stream << info._length;
        stream << info._attrib;
        return stream;
    }
    friend QDataStream &operator >> (QDataStream &stream, BriefFileInfo info)
    {
        stream >> info._name;
        stream >> info._lastChanged;
        stream >> info._created;
        stream >> info._length;
        stream >> info._attrib;
        return stream;
    }


protected:
    QString     _name;                                      // имя файла локальное
    QDateTime   _lastChanged;                               // дата изменения
    QDateTime   _created;                                   // дата создания
    qint64      _length;                                    // длина
    QString     _attrib;                                    // атрибуты
};


class Responce
{
public:

    Responce()  {}
    ~Responce() {}
    virtual QByteArray data() { return QByteArray((const char*)this, sizeof(RemoteRq)); }
protected:
    RemoteRqType rq;                                        // запрос
    QHostAddress src;                                       // IP источника лответа
    QHostAddress dst;                                       // IP назначение ответа
//  QString      remotePath;                                // IP удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост  tcp://10.52.19.71/tcp://192.168.1.1
    QString      reserv1;                                   // резерв
    int          reserv2;                                   // резерв
};


#endif // REMOTERQ_H
