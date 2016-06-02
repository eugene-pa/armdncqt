#ifndef RESPONCEDIRS_H
#define RESPONCEDIRS_H

#include <QDir>
#include <QStringList>
#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceDirs
{
public:
    ResponceDirs();                                         // конструктор по умолчанию для приемной стороны
    ResponceDirs(RemoteRq& req);                            // конструктор на базе запроса
    ~ResponceDirs();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);

    QString toString();
    RemoteRq        rq() { return _rq; }
    bool            exist() { return _exist; }
    QStringList     list() { return _list; }
    QString         folder() { return _folder; }

protected:
    RemoteRq        _rq;                                    // исходный запрос
    QString         _folder;
    bool            _exist;                                 // папка существует
    QStringList     _list;                                  // список файлов заданной папки
};

#endif // RESPONCEDIRS_H
