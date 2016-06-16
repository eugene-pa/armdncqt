#ifndef HEADERRESPONCE_H
#define HEADERRESPONCE_H

#include "remoterq.h"

class HeaderResponce
{
public:
    HeaderResponce() { }                                    // конструктор по умолчанию
    HeaderResponce(RemoteRq& req, bool success=true);       // конструктор на базе запроса
    ~HeaderResponce();

    void Serialize  (QDataStream &stream);
    void Deserialize(QDataStream &stream);

    RemoteRqType Rq() { return rq; }

    void setsrc(QHostAddress a) { src = a; }                // IP источника запроса
    void setdst(QHostAddress a) { dst = a; }                // IP назначение запроса

    bool success() { return _success; }                     // все ОК
    bool error  () { return !_success; }                    // ошибка

    void setError(bool s=true) { _success = s; }

protected:

    RemoteRqType rq;                                        // тип запроса
    bool         _success;                                  // успех/ошибка
    QHostAddress src;                                       // IP источника ответа
    QHostAddress dst;                                       // IP назначение ответа
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: tcp://10.52.19.71/tcp://192.168.1.1
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв
};

#endif // HEADERRESPONCE_H
