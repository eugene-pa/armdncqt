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
    ResponceTempFile(RemoteRq& req);                        // конструктор на базе запроса (для передающей стороны)
    ~ResponceTempFile();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();
    bool exist() { return _exist; }
protected:
    QTemporaryFile temp;                                    //
    RemoteRq        _rq;                                    // исходный запрос

    QString         _filesrc;                               // оригинальный файл
    bool            _exist;                                 // папка существует
    QString         _filetemp;                              // временная копия
};

#endif // RESPONCETEMPFILE_H
