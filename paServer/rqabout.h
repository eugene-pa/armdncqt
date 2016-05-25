#ifndef RQABOUT_H
#define RQABOUT_H

#include "remoterq.h"

class RqAbout : public RemoteRq
{
public:
    RqAbout();
    ~RqAbout();

protected:


    QString     filedName;                                      // 1 - исполняемый файл сервера
    QFileInfo   fileInfo;                                       // 2 - информация о файле
//    [0].Assembly.GetName().Version.ToString());                   // 3 - версия сборки
//    MachineName);                                          // 4 - имя ПК
//    OSVersion);                                            // 5 - информация об ОС
//    ProcessorCount);                                       // 6 - число процессоров
//    UserName);                                             // 7 - учетная запись (системная)
//    UserDomainName);                                       // 8 - имя домена     (системный)
//    Version);                                              // 9 - версия .NET
//    WorkingSet);                                           // 10- номер WorkingSet

//    sb.Append("\r\n").Append("FileServer #").Append(Number);                                    // 11- номер объекта
//    sb.Append("\r\n").Append("Total FileServers: ").Append(GetObjectCounter());                 // 12- число серверных объектов

};

#endif // RQABOUT_H
