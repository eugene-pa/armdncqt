#ifndef RQABOUT_H
#define RQABOUT_H

#include <QCoreApplication>
#include <QTextCodec>
#include "remoterq.h"

class RqAbout : public RemoteRq
{
public:
    RqAbout();
    ~RqAbout();
    QByteArray Serialize();
protected:
};


class ResponceAbout
{
public:
    ResponceAbout();
    QByteArray Serialize();
protected:
    QString         fileName;                               // - исполняемый файл сервера
    BriefFileInfo   fileInfo;                               // - информация об исполняемом файле
    QString         hostName;                               // - имя ПК
    QString         cpu;                                    // - тип процессора
    QString         kernel;                                 // - тип ядра ОС
    QString         version;                                // - версия ядра ОС
    QString         userName;                               // - учетная запись (системная)
    QString         reserv3;
    int             reserv4;
};

#endif // RQABOUT_H


