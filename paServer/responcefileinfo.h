#ifndef RESPONCEFILEINFO_H
#define RESPONCEFILEINFO_H

#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceFileInfo
{
public:
    ResponceFileInfo();                                     // конструктор по умолчанию для приемной стороны
    ResponceFileInfo(RemoteRq& req);                        // конструктор на базе запроса
    ~ResponceFileInfo();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();

protected:
    RemoteRq        _rq;                                    // исходный запрос
    QString         _filepath;                              // искомый файл

    // сериализуемая часть
    bool            _exist;                                 // файл существует
    BriefFileInfo   _fileInfo;                              // информация о файле
};

#endif // RESPONCEFILEINFO_H
