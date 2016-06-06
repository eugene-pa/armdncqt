#ifndef REMOTERQ_H
#define REMOTERQ_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>
#include <QBuffer>
#include "../common/defines.h"

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
    friend class HeaderResponce;
    friend class ResponceAbout;
    friend class ResponceDirs;
    friend class ResponceFileInfo;
    friend class ResponceFiles;
    friend class ResponceTempFile;
public:

    static QHostAddress localaddress;
    static QHostAddress remoteaddress;

    RemoteRq(RemoteRqType req = rqEmpty);
    ~RemoteRq();

    static const quint32 streamHeader;                      // заголовок
    static const quint16 paServerVersion;                   // версия paServer

    bool isRemote() { return remotePath.length() > 0; }     // надо более строго выделить корректный удаленный путь

    QByteArray Serialize();
    void Deserialize(QDataStream &stream);

//    void SerializeBase(QDataStream &stream);
//    void DeserializeBase(QDataStream &stream);

    RemoteRqType Rq() { return rq; }

    void setParam (QVariant value) { param  = value; }
    void setParam2(QVariant value) { param2 = value; }
    void setParam3(QVariant value) { param3 = value; }

protected:
    // несериализуемая часть
    quint32 header;                                         // сигнатура сериализации
    quint16 version;                                        // версия протокола

    // сериализуемая часть
    RemoteRqType rq;                                        // запрос
    QHostAddress src;                                       // IP источника запроса
    QHostAddress dst;                                       // IP назначение запроса
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: tcp://10.52.19.71/tcp://192.168.1.1
    QString      remotePath;                                // удаленный хост (возможен рекурсивный путь); если пустая строка - локальный хост
    QVariant     param;                                     // параметр
    QVariant     param2;                                    // параметр
    QVariant     param3;                                    // параметр
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв
};

/*
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
    QVariant      reserv1;                                   // резерв
    QVariant      reserv2;                                   // резерв
};
*/

#endif // REMOTERQ_H
