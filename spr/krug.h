
#ifndef KRUG_H
#define KRUG_H

#include "qstring.h"
#include "qhash.h"

class KrugInfo
{
public:

    // статические члены
    static QList<KrugInfo*> Krugs;                          // список кругов, отсортированный по именам
    static QHash<int, KrugInfo*> KrugsById;                 // словарь кругов по номерам

    // открытые функции
    KrugInfo();
    ~KrugInfo();

    int     no    (){ return _no;       }                   // численный идентификатор (условный номер _id) круга
    QString name  (){ return _name;     }                   // наименование круга
    QString dbpath(){ return _dbpath;   }                   // наименование круга
    QString dbdir (){ return _dbdir;    }                   // путь к папке БД
    QString projectpath() { return _projectpath; }          // путь к папке проекта
    QString inipath      () { return _inipath;       }      // путь к хранению INI файлов
    QString rootDirBridge() { return _rootDirBridge; }      // путь на шлюзе (примеры: c:/armdnc, c:/armdnc_2)
    QString rootDirArmDnc() { return _rootDirArmDnc; }      // путь на АРМ ДНЦ (примеры: c:/armdnc, c:/armdnc_2)
    QString ipmain           (){ return _ipMain           ;}// ip-адрес:ПОРТ основного шлюза
    QString ipMainWithoutPort(){ return _ipMainWithoutPort;}
    QString ipRsrvWithoutPort(){ return _ipRsrvWithoutPort;}

    QList <class Train*> trains() { return _trains; }       // информация о поездах круга

protected:
    int     _no;                                            // числовой номер/идентификаторр/ключ объекта
    QString _name;                                          // имя объекта
    QString _dbpath;                                        // путь к БД НСИ
    QString _dbdir;                                         // путь к папке БД
    QString _projectpath;                                   // путь к папке БД
    QString _inipath;                                       // путь к хранению INI файлов
    QString _rootDirBridge;                                 // путь на шлюзе (примеры: c:/armdnc, c:/armdnc_2)
    QString _rootDirArmDnc;                                 // путь на АРМ ДНЦ (примеры: c:/armdnc, c:/armdnc_2)
    QString _ipMain;                                        // ip-адрес:ПОРТ основного шлюза
    QString _ipMainWithoutPort;
    QString _ipRsrvWithoutPort;

    QList <class Train*> _trains;                           // информация о поездах круга

//    public ArrayList DataRcInfo = new ArrayList();                          // информация об РЦ круга
//    public List<PCInfo> Hosts   { get; private set; }                       // список хостов

//    public bool MonitoringOff { get; set; }                                 // признак временного выключения объекта из мониторинга

//    public override string ToString()
//    {
//        return AliasRegion + ". " + Name;
//    }

};

#endif // KRUG_H
