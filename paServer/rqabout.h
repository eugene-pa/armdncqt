#ifndef RQABOUT_H
#define RQABOUT_H

#include "remoterq.h"

class RqAbout : public RemoteRq
{
public:
    RqAbout() ;
    ~RqAbout();

protected:


    QString         filedName;                               // 1 - исполняемый файл сервера
    BriefFileInfo   fileInfo;                                // 2 - информация об исполняемом файле
    QString         hostName;                                // 4 - имя ПК
    QString         osVersion;                               // 5 - информация об ОС
    QString         userName;                                // 7 - учетная запись (системная)
    QString         reserv3;
    int             reserv4;
};

#endif // RQABOUT_H
