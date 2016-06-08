#ifndef RESPONCEREAD_H
#define RESPONCEREAD_H

#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceRead
{
public:
    ResponceRead();                                         // конструктор по умолчанию для приемной стороны
    ResponceRead(RemoteRq& req, Logger * logger=nullptr);   // конструктор на базе запроса
    ~ResponceRead();
    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();

    quint64 offset() { return _offset; }
    QByteArray& data() { return _data; }
    QString dstfilepath() { return _dstfilepath; }
    QString srcfilepath() { return _srcfilepath; }
    bool isEof() { return _eof; }
    int length() { return _length; }
protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
    QString         _srcfilepath;                           // копируемый файл
    QString         _dstfilepath;                           // целевой файл
    bool            _exist;                                 // файл существует
    bool            _eof;                                   // достигнут конец файла
    int             _length;                                // длина считанных данных
    quint64         _offset;
    BriefFileInfo   _fileInfo;                              // информация о файле
    QByteArray      _data;                                  // считанные данные


};

#endif // RESPONCEREAD_H
