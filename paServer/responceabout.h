#ifndef RESPONCEABOUT_H
#define RESPONCEABOUT_H

#include "rqabout.h"

class ResponceAbout
{
public:
    ResponceAbout();
    ~ResponceAbout();

    QByteArray Serialize();
    void Deserialize(QByteArray& data);

protected:
    QString         fileName;                               // - исполняемый файл сервера
    BriefFileInfo   fileInfo;                               // - информация об исполняемом файле
    QString         hostName;                               // - имя ПК
    QString         cpu;                                    // - тип процессора
    QString         kernel;                                 // - тип ядра ОС
    QString         osversion;                              // - версия ядра ОС
    QString         userName;                               // - учетная запись (системная)
    QVariant        reserv3;
    QVariant        reserv4;

};

#endif // RESPONCEABOUT_H
