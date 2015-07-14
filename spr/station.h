#ifndef STATION_H
#define STATION_H

#include <QObject>

#include "properties.h"
#include "../common/logger.h"
#include "ts.h"
#include "tu.h"
#include "rc.h"
#include "strl.h"
#include "svtf.h"
#include "route.h"
#include "peregon.h"
#include "enums.h"
#include "streamts.h"
#include "datafrommonitor.h"
#include "sysinfo.h"
#include "dras.h"
#include "datafrommonitor.h"
#include "dstdatafromfonitor.h"


class Station : public QObject
{
    friend class DStDataFromMonitor;

    Q_OBJECT

public:
    // открытые статические члены
    static QHash<int, Station*> Stations;                   // хэш-таблица указателей на справочники станций
    static bool LockLogicEnable;                            // включен логический контроль
    static short	MainLineCPU;                            // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. основного канала связи
    static short	RsrvLineCPU;                            // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. обводного канала связи

    // открытые статические функции
    static Station * GetById(int no);                       // получить справочник по номеру станции
    static Station * GetSprByNoOrgAndKrug(int no, int bridgeno);
    static Station * GetByName(QString stname);             // получить справочник по номеру станции
    static bool ReadBd (QString& dbpath, Logger& logger);   // чтение БД
    static void SortTs();                                   // сортировка списка ТС
    static void SortTu();                                   // сортировка списка ТУ
    static void ParsePrologEpilog(Logger& logger);          // "разрешить" ссылки ПРОЛОГ/ЭПИЛОГ/ПОЛЮС
    // открытые функции
    Station(QSqlQuery& query, Logger& logger);              // конструктор на базе записи в БД
    ~Station();


    QString& Name() { return  name; }
    void AddTs (class Ts*, Logger& logger);                 // добавить ТС
    void AddTu (class Tu*, Logger& logger);                 // добавить ТУ
    void GetTsParams (int& maxModul, int& maxI, int& maxJ, int& tsPerModule);

    bool IsMpcEbilock() { return mpcEbilock; }              // конфигурация с Ebilock950, а также РПЦ/МПЦ МПК + rpcMpcMPK
    bool IsRpcMpcMPK () { return rpcMpcMPK; }               // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool IsRpcDialog () { return rpcDialog; }               // конфигурация с РПЦ Диалог
    bool IsApkdk     () { return apkdk;     }               // конфигурация с АПКДК
    bool IsAdkscb    () { return adkScb;    }               // конфигурация с АДКСЦБ
    bool IsUpokotu   () { return upokOtu;   }               // конфигурация с УПОК
    bool Kp2000Lomikont() { return version == VERSIONKp2000Lomikont ; }
    bool Kp2000        () { return version == VERSION2000           ; }
    bool Kp2000Tums    () { return version == VERSION2000Rpctums    ; }
    bool Kp2007        () { return version == VERSION2007           ; }
    bool Retime        () { return version == VERSIONRetime         ; }
    bool KpPa          () { return Kp2007() || Kp2000() || Kp2000Lomikont() || Kp2000Tums(); }

    int  Version    () { return version;    }
    bool Enable     () { return enable;     }
    bool Actual     () { return stsActual;  }               // станция выбрана ДНЦ для работы
    bool ByByLogic  () { return bybylogic;  }               // логика удалений (У1,У2)
    bool IsAu       () { return stsAu;      }
    bool IsSu       () { return stsSu;      }
    bool IsRu       () { return stsRu;      }
    bool IsMu       () { return stsMu;      }
    bool IsOn       () { return stsOn;      }
    bool IsRsrv     () { return stsRsrv;    }               // КП на резерве
    bool IsCom3On   () { return stsCom3On;  }
    bool IsCom4On   () { return stsCom4On;  }
    bool IsBackChannel() { return stsBackChannel; }         // последний опрос станции по обратному каналу

    bool IsActualErrorLockMsgPresent() { return errorLockLogicCount; }  // число актуальных ошибок логического контроля
    bool IsNewErrorLockMsgPresent   ();                                 // есть ли неквитированные сообщения подсистемы логич.контроля
    bool IsTsExpire();                                       // устарели ли ТС

    // доступ к состоянию ТС по смещению ТС в массиве
    bool TsSts      (int i)
        { return TsStsMoment(i) && !TsPulse(i); }            // состояние ТС (1 - если не мигает и в единице)
    bool TsStsMoment(int i) { return TestBit(tsSts,     i); }// состояние ТС по моменту
    bool TsRaw      (int i) { return TestBit(tsStsRaw,  i); }// состояние не нормализованное
    bool TsPulse    (int i) { return TestBit(tsStsPulse,i); }// состояние пульсации

    int MtsCount () { return mts.count(); }                 // число модулей ТС
    int MtuCount () { return mtu.count(); }                 // число модулей ТУ

    bool SetBit (QBitArray& bits, int index, bool a=true);  // установить бит в заданном массиве в заданное состояние (по умолчанию в 1)
    void MarkInverse(int index);

    void AddRc  (class Rc   *, Logger& logger);             // добавить РЦ
    void AddSvtf(class Svtf *, Logger& logger);             // добавить СВТФ
    void AddStrl(class Strl *, Logger& logger);             // добавить СТРЛ
    void AddRoute(Route* route);                            // добавить маршрут

    Route * GetRouteByNo(int no);                           // получить маршрут по номеру маршрута на станции

// вычисление переменной - через обработку сигнала в слоте
public slots:
   void GetValue(QString& name, int& ret);                    // вычисление переменной в выражении формата ИМЯ_ТС[ИМЯ_ИЛИ_#НОМЕР_СТАНЦИИ]

private:
   // закрытые члены - таблицы ТС по станции
    QHash <QString, class Ts*> Ts;                          // индексированы по текстовому имени ТС
    QHash <int, class Ts*> TsIndexed;                       // индексированы по индексу ТС
    QHash <int, class Ts*> TsByIndxTsName;                  // индексированы по индексу имени
    QList <class Ts*> TsSorted;                             // отсортированы по имени

    // закрытые члены - таблицы ТУ по станции
    QHash <QString, class Tu*> Tu;                          // индексированы по текстовому имени ТУ
    QHash <int    , class Tu*> TuByIJ;                      // индексированы по IJ
    QList <class Tu*> TuSorted;                             // отсортированы по имени

    QHash <int, class Rc  *> allrc;                         // РЦ станции, индексированные по индексу ТС
    QHash <int, class Svtf*> allsvtf;                       // РЦ станции, индексированные по индексу ТС
    QHash <int, class Strl*> allstrl;                       // РЦ станции, индексированные по индексу ТС

    QList <class ShapeId*> formList;                        // список классов-идентификаторов форм

    QHash <int, Route *> routes;                            // маршруты на станции, индексированные по номеру маршрута на станции

    int     no;                                             // номер
    QString noext;                                          // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]
    QString name;
    int     ras;                                            // номер станции связи
    int     addr;                                           // линейный адрес
    int     krugId;                                         // номер круга
    int     version;                                        // версия конфигурации станции (KpVersion)
    int     otuAddr;                                        // адрес в подсистеме ОТУ (СПОК/БРОК)
    QString config;                                         // строка конфигурации
    int     kpIdBase;                                       // ID КП, явно заданный в БД или 0
    int     esr;                                            // код ЕСР
    int     gidUralId;                                      // идентификация в ГИД УРАЛ в локальной НСИ
    int     gidUralIdRemote;                                // идентификация в ГИД УРАЛ, полученная из удаленного АРМ
    bool    enable;                                         // OnOff
    bool    du;                                             // станция ДУ
    int     bitmapIdRss;                                    //
    int     radioIdRss;                                     //
    int     radioIdMnt;                                     //
    int     T[4];                                           // коэффициенты связи
    int     forms;                                          // число форм
    QString extForms;                                       // доп.формы
    QString orient;                                         // ориентация

    bool    mpcEbilock;                                     // конфигурация с Ebilock950, а также РПЦ/МПЦ МПК + rpcMpcMPK
    bool    rpcMpcMPK;                                      // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool    rpcDialog;                                      // конфигурация с РПЦ Диалог
    bool    apkdk;                                          // конфигурация с АПКДК
    bool    adkScb;                                         // конфигурация с АДКСЦБ
    bool    upokOtu;                                        // конфигурация с УПОК
    bool    bybylogic;                                      // логика удалений (У1,У2)


    QByteArray     mts;                                     // массив номеров модулей ТС
    QByteArray     mtu;                                     // массив номеров модулей ТУ

    // состояние сигналов ТС
    QBitArray tsInverse;                                    // битовый массив инверсии
    QBitArray tsStsRaw;                                     // текущий съем с объекта позиционного состояния сигналов в сыром виде
    QBitArray tsStsPulse;                                   // текущий съем с объекта состояния мигания в сыром виде
    QBitArray tsStsRawPrv;                                  // позиционное состояние на пред.шаге
    QBitArray tsStsPulsePrv;                                // мигание на пред.шаге
    QBitArray tsSts;                                        // обработанный массив ТС

    // динамические переменные состояния объекта
    bool    stsActual;                                      // станция выбрана ДНЦ для работы
    bool    stsAu;
    bool    stsSu;
    bool    stsRu;
    bool    stsMu;
    bool    stsOn;
    bool    stsRsrv;                                        // КП на резерве (по состоянию активного блока)
    bool    stsCom3On;
    bool    stsCom4On;
    bool    stsBackChannel;                                 // последний опрос станции по обратному каналу

    int     errorLockLogicCount;                            // число актуальных ошибок логического контроля
    bool    stsFrmMntrErrorLockMsgPresent;					// наличие ЗЦ.ОШБ в базовом удаленном АРМ
    bool    stsFrmMntrTsExpired;							// ТС устарели в базовом удаленном АРМ

    class SysInfo * mainSysInfo;                            // блок сист.информации основного  БМ
    class SysInfo * rsrvSysInfo;                            // блок сист.информации резервного БМ
    time_t  tSpokSnd;                                       // время передачи данных в СПОК
    time_t  tSpokRcv;                                       // время приема данных от СПОК

    // можно объявить экземпляр класса DStDataFromMonitor, чтобы хранить тут сформированные или полученные данные потока
    // это можно было бы сделать, чтобы избежать полного разбора потока при приеме, просто скопировав данные (наложив шаблон класса)
    // не факт, что игра стоит свеч, так как усложняется определение состояний

    int realStreamTsLength;									// Реальная длина данных ТС в блоке DtFrmMnt из расчета 1 бит на сигнал (192, 384, 512...)
    QString   buf;                                          // строка сообщений

    // закрытые функции
    bool parseNames (QString& srcname, Station*& st, QString& name); // разбор индексированных имен ТУ/ТС
    bool TestBit (QBitArray& bits, int index);              // проверка бита в битовом массиве
    void ParseExtForms();                                   // разбор доп.форм
};

// класс идентификации формы станции (имя формы, ID кнопки)
// в общем случае станция может иметь несколько форм для представления схемы
// чаще всего - одну форму, иногда - ни одной
class ShapeId
{
public:
    ShapeId(Station * st, QString& name, int radioid)
    {
        this->st = st;
        this->radioid = radioid;
        this->name = name;
        fileName = name + ".shp";
    }
private:
    Station * st;
    int radioid;
    QString name;
    QString fileName;
};

#endif // STATION_H
