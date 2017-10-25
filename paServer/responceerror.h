#ifndef RESPONCEERROR_H
#define RESPONCEERROR_H

#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

enum ResponceErrorType
{
    Ok              = 0,
    Disconnect,                                             // ошибка соединения
    Timeout,                                                // таймаут
    Unknown,                                                // неизвестная ошибка
};

class ResponceError
{
public:
    ResponceError();                                        // конструктор по умолчанию для приемной стороны
    ResponceError(RemoteRq& req, ResponceErrorType error, Logger * logger=nullptr);  // конструктор на базе запроса
    ~ResponceError();

    static QString getErrorName(RemoteRqType);              // получить имя ошибки

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString errorName();
    QString toString();

    ResponceErrorType error() { return _error; }
    void setErrorText(QString s) { _errorText = s; }

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
    ResponceErrorType _error;
    QString         _errorText;

};

#endif // RESPONCEERROR_H
