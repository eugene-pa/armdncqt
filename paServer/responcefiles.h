#ifndef RESPONCEFILES_H
#define RESPONCEFILES_H

#include <QVector>
#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceFiles
{
public:
    ResponceFiles();                                        // конструктор по умолчанию для приемной стороны
    ResponceFiles(RemoteRq& req, Logger * logger);          // конструктор на базе запроса (для передающей стороны)
    ~ResponceFiles();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();

    QVector <BriefFileInfo> files() { return _files; }

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QString         _folder;                                // папка
    bool            _exist;                                 // папка существует

    QVector <BriefFileInfo> _files;                         // информация о файлах заданного каталога
};

#endif // RESPONCEFILES_H
