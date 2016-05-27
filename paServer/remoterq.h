#ifndef REMOTERQ_H
#define REMOTERQ_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>

// удаленный запрос

enum RemoteRqType
{
    rqEmpty     = 0,
    rqAbout     = 1,                                    // информация о версии сервиса и хосте
    rqDirInfo   = 2,                                    // информация о каталоге: дата создания, число файлов, число папок, размер
    rqDirSize   = 3,                                    // размер каталога в байтах
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

    virtual QByteArray data() { return QByteArray((const char*)this, sizeof(RemoteRq)); }

protected:
    RemoteRqType rq;                                        // запрос
    QString      host;                                      // удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост  tcp://10.52.19.71/tcp://192.168.1.1
    QString      reserv1;                                   // резерв
    int          reserv2;                                   // резерв
};

class BriefFileInfo
{
public:
    BriefFileInfo() { }
    BriefFileInfo (QFileInfo& fi);                          // конструктор
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

    Responce();
    ~Responce();
    virtual QByteArray data() { return QByteArray((const char*)this, sizeof(RemoteRq)); }
protected:
    RemoteRqType rq;                                        // запрос
    QHostAddress src;                                       // IP источника запроса
    QHostAddress dst;                                       // IP удаленной стороны
    QString      remotePath;                                // IP удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост  tcp://10.52.19.71/tcp://192.168.1.1
    QString      reserv1;                                   // резерв
    int          reserv2;                                   // резерв
};


#endif // REMOTERQ_H
