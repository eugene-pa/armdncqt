#ifndef RESPONCEFILEINFO_H
#define RESPONCEFILEINFO_H

#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceFileInfo
{
public:
    ResponceFileInfo();                                     // конструктор по умолчанию для приемной стороны
    ResponceFileInfo(RemoteRq& req, Logger * logger=nullptr);// конструктор на базе запроса
    ~ResponceFileInfo();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    bool isDir() { return _dir; }
    QString toString();
    BriefFileInfo fileInfo() { return _fileInfo; }

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;
    QString         _filepath;                              // искомый файл

    // сериализуемая часть
    bool            _exist;                                 // файл существует
    bool            _dir;                                   // каталог
    BriefFileInfo   _fileInfo;                              // информация о файле
};

#endif // RESPONCEFILEINFO_H
