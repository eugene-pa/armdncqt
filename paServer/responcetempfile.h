#ifndef RESPONCETEMPFILE_H
#define RESPONCETEMPFILE_H

#include <QTemporaryFile>
#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"


class ResponceTempFile
{
public:
    ResponceTempFile();                                     // конструктор по умолчанию для приемной стороны
    ResponceTempFile(RemoteRq& req, Logger * logger=nullptr);// конструктор на базе запроса (для передающей стороны)
    ~ResponceTempFile();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();
    bool exist() { return _exist; }
    QString& name() { return _filetemp; }
    QString& nameSrc() { return _filesrc; }

    static QStringList trashTempFiles;                      // список временных файлов, созданных за время работы

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    QString         _filesrc;                               // оригинальный файл
    bool            _exist;                                 // папка существует
    QString         _filetemp;                              // временная копия
};

#endif // RESPONCETEMPFILE_H
