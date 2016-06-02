#ifndef HEADERRESPONCE_H
#define HEADERRESPONCE_H

#include "remoterq.h"

class HeaderResponce
{
public:
    HeaderResponce() { }                                    // конструктор по умолчанию
    HeaderResponce(RemoteRq& req);                          // конструктор на базе запроса
    ~HeaderResponce();

    void Serialize  (QDataStream &stream);
    void Deserialize(QDataStream &stream);

    RemoteRqType Rq() { return rq; }
protected:
    RemoteRqType rq;                                        // тип запроса

    // сериализуемая часть клпсса
    QHostAddress src;                                       // IP источника ответа
    QHostAddress dst;                                       // IP назначение ответа
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: tcp://10.52.19.71/tcp://192.168.1.1
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв
};

#endif // HEADERRESPONCE_H
