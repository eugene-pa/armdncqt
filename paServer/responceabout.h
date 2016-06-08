#ifndef RESPONCEABOUT_H
#define RESPONCEABOUT_H

#include "remoterq.h"
#include "headerresponce.h"
#include "brieffileinfo.h"

class ResponceAbout
{
public:
    ResponceAbout();                                        // конструктор по умолчанию для приемной стороны
    ResponceAbout(RemoteRq& req, Logger * logger=nullptr);  // конструктор на базе запроса
    ~ResponceAbout();

    QByteArray Serialize();
    void Deserialize(QDataStream& stream);
    QString toString();

protected:
    RemoteRq        _rq;                                    // исходный запрос
    Logger *        _logger;

    // сериализуемая часть
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
