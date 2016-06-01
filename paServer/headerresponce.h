#ifndef HEADERRESPONCE_H
#define HEADERRESPONCE_H

#include "rqabout.h"

class HeaderResponce
{
public:
    HeaderResponce();
    ~HeaderResponce();

    void Serialize  (QDataStream &stream);
    void Deserialize(QDataStream &stream);

protected:
    RemoteRqType rq;                                        // тип запроса
    QHostAddress src;                                       // IP источника ответа
    QHostAddress dst;                                       // IP назначение ответа
    QString      fullPath;                                  // полный константный путь запроса, возможно рекурсивный: tcp://10.52.19.71/tcp://192.168.1.1
    QVariant     reserv1;                                   // резерв
    QVariant     reserv2;                                   // резерв
    QVariant     reserv3;                                   // резерв
};

#endif // HEADERRESPONCE_H
