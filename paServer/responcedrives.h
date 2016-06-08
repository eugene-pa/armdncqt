#ifndef RESPONCEDRIVES_H
#define RESPONCEDRIVES_H

#include <QDir>
#include <QStringList>
#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceDrives
{
public:
    ResponceDrives();                                       // конструктор по умолчанию для приемной стороны
    ResponceDrives(RemoteRq& req, Logger * logger=nullptr); // конструктор на базе запроса
    ~ResponceDrives();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

    QString toString();
    QStringList     list() { return _list; }

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QStringList     _list;                                  // список дисков
};

#endif // RESPONCEDRIVES_H
