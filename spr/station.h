#ifndef STATION_H
#define STATION_H

#include <QObject>

#include "properties.h"
#include "../common/defines.h"
#include "../common/boolexpression.h"
#include "../common/logger.h"
#include "ts.h"
#include "tu.h"
#include "otu.h"
#include "rc.h"
#include "strl.h"
#include "svtf.h"
#include "route.h"
#include "train.h"
#include "peregon.h"
#include "enums.h"
#include "streamts.h"
#include "datafrommonitor.h"
#include "sysinfo.h"
#include "dras.h"
#include "datafrommonitor.h"
#include "dstdatafromfonitor.h"
#include "sysinfo.h"


class Station : public QObject
{
    friend class DStDataFromMonitor;
    friend class Route;

    Q_OBJECT

public:
    // открытые статические члены
    static QHash<int, Station*> Stations;                   // хэш-таблица указателей на справочники станций
    static bool LockLogicEnable;                            // включен логический контроль
    static bool InputStreamRss;                             // тип входного потока: InputStreamRss=true-Станция связи, false-Управление
    static short	MainLineCPU;                            // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. основного канала связи
    static short	RsrvLineCPU;                            // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. обводного канала связи

    static bool FastScanArchive;                            // быстрый просмотр архива - данные по объектам не обрабатываются

    // открытые статические функции
    //static Station * GetById(int no);                       // получить справочник по номеру станции
    static Station * GetById(int no, class KrugInfo * krug = nullptr);
    static Station * GetByName(QString stname);             // получить справочник по номеру станции
    static bool ReadBd (QString&, class KrugInfo*, Logger&);// чтение БД
    static void SortTs();                                   // сортировка списка ТС
    static void SortTu();                                   // сортировка списка ТУ
    static void ParsePrologEpilog(Logger& logger);          // "разрешить" ссылки ПРОЛОГ/ЭПИЛОГ/ПОЛЮС
    static void CountMT();                                  // посчитать модули и сдвинуть пометки МТС для КП2000 на число ТУ

    // открытые функции
    Station(QSqlQuery&, class KrugInfo* , Logger& );        // конструктор на базе записи в БД
    ~Station();


    int  No     () { return no; }                           // #
    QString& Name(){ return name; }                         // имя
    int  Ras    () { return ras; }                          // #RAS
    int  Addr   () { return addr;}                          // линейный адрес
    int  GidUralId()  { return gidUralId; }                 // идентификация в ГИД УРАЛ в локальной НСИ
    int  GidRemoteId(){ return gidUralIdRemote; }           // идентификация станции в потоке
    QString& Config() { return config; }                    // конфигурация КП

    void AddTs (class Ts*, Logger& logger);                 // добавить ТС
    void AddTu (class Tu*, Logger& logger);                 // добавить ТУ
    void GetTsParams (int& maxModul, int& maxI, int& maxJ, int& tsPerModule);

    bool IsMpcEbilock() { return mpcEbilock; }              // конфигурация с Ebilock950, а также РПЦ/МПЦ МПК + rpcMpcMPK
    bool IsRpcMpcMPK () { return rpcMpcMPK; }               // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool IsRpcDialog () { return rpcDialog; }               // конфигурация с РПЦ Диалог
    bool IsApkdk     () { return apkdk;     }               // конфигурация с АПКДК
    bool IsAbtcm     () { return abtcm;     }               // конфигурация с АБТЦМ
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

    void AcceptTS   ();                                     // обработка данных

    bool IsDu       () { return !stsAu && !stsSu && !stsRu; } // ДУ
    bool IsAu       () { return stsAu;      }
    bool IsSu       () { return stsSu;      }
    bool IsRu       () { return stsRu;      }
    bool IsMu       () { return stsMu;      }
    bool IsOn       () { return stsOn;      }
    bool IsRsrv     () { return stsRsrv;    }               // КП на резерве
    bool IsCom3On   () { return stsCom3On;  }
    bool IsCom4On   () { return stsCom4On;  }
    bool IsBackChannel() { return stsBackChannel; }         // последний опрос станции по обратному каналу
    class SysInfo * GetSysInfo(bool rsrv) { return rsrv ?  rsrvSysInfo : mainSysInfo; } // блок сист.информации основного  БМ
    bool IsSupportKpExt (bool rsrv);                        // поддерживается ли расширенный блок диагностики
    bool IsAtuError (bool rsrv);                            // проверка срабатывания АТУ (отличается в разных версиях)
    bool IsKeyError (bool rsrv);                            // проверка ошибки ключа     (отличается в разных версиях)
    bool IsOutError (bool rsrv);                            // проверка ошибки выхода    (отличается в разных версиях)
    bool IsTestMode (bool rsrv);                            // проверка выполнения теста
    bool IsWatchdogOn(bool rsrv);                           // проверка включения сторожевого таймера
    bool IsMemError (bool rsrv);                            // проверка ошибки использования памяти
    bool IsOtuLineOk(bool rsrv);                            // проверка готовности системы ОТУ
//  bool IsOtuBrokOn(bool rsrv);                            // проверка готовности УПОК/БРОК
    bool IsRetrans  (bool rsrv);                            // проверка режима ретрансляции
    bool IsConsol   (bool rsrv);                            // проверка подключения отладочной консоли
    bool IsOtuPending(bool rsrv);                           // проверка флага исполнения ОТУ
    bool IsDebugOtuMode(bool rsrv);                         // проверка включения режима отладки ОТУ
    bool IsArmDspModeOn(bool rsrv);                         // проверка режима АРМ ДСП
    QDateTime GetLastTime (bool rsrv);                      // получить время последнего опроса нужного блока

    bool IsActualErrorLockMsgPresent() { return errorLockLogicCount; }  // число актуальных ошибок логического контроля
    bool IsNewErrorLockMsgPresent   ();                                 // есть ли неквитированные сообщения подсистемы логич.контроля
    bool IsTsExpire();                                       // устарели ли ТС

    // доступ к состоянию ТС по смещению ТС в массиве
    bool TsSts      (int i)
        { return TsStsMoment(i) && !TsPulse(i); }            // позиционное состояние ТС (1 - если не мигает и в единице)
    bool TsStsMoment(int i) { return TestBit(tsSts,     i); }// состояние ТС по моменту
    bool TsRaw      (int i) { return TestBit(tsStsRaw,  i); }// состояние не нормализованное
    bool TsPulse    (int i) { return TestBit(tsStsPulse,i); }// состояние пульсации

    int MtsCount () { return mtsCount; }                    // число модулей ТС
    int MtuCount () { return mtuCount; }                    // число модулей ТУ

    void SetBit (QBitArray& bits, int index, bool a=true);  // установить бит в заданном массиве в заданное состояние (по умолчанию в 1)
    void MarkInverse(int index);
    void MarkTs (int n) { SetBit(mts, n-1); }               // пометить модуль ТС как существующий
    void MarkTu (int n) { SetBit(mtu, n-1); }               // пометить модуль ТУ как существующий

    void AddRc  (class Rc   *, Logger& logger);             // добавить РЦ
    void AddSvtf(class Svtf *, Logger& logger);             // добавить СВТФ
    void AddStrl(class Strl *, Logger& logger);             // добавить СТРЛ
    void AddRoute(Route* route);                            // добавить маршрут

    Route * GetRouteByNo(int no);                           // получить маршрут по номеру маршрута на станции

    QHash <int, class Rc  *> & Allrc  () { return allrc;  } // РЦ станции, индексированные по индексу ТС
    QHash <int, class Svtf*> & Allsvtf() { return allsvtf;} // РЦ станции, индексированные по индексу ТС
    QHash <int, class Strl*> & Allstrl() { return allstrl;} // РЦ станции, индексированные по индексу ТС
    QHash <int, class Route*>& Allroute(){ return routes; } // маршруты на станции, индексированные по номеру маршрута на станции

    class Ts * GetTsByIndex(int indx);                      // получить ТС по индексу
    bool GetTsStsByIndex     (int indx);                    // получить состояние сигнала в марице ТС
    bool GetTsPulseStsByIndex(int indx);                    // получить состояние мигания сигнала в марице ТС
    bool GetTsStsRawByIndex  (int indx);                    // получить оригинальное состояние сигнала в марице ТС

    bool GetTsStsByName      (QString name);                // получить состояние сигнала по имени
    bool GetTsPulseStsByName (QString name);                // получить состояние мигания сигнала по имени
    int  GetTsStsByNameEx    (QString name);                // d0 - состояние, d1 - мигание

    // таблицы ТС по станции (перенести в protected)
    QHash <QString, class Ts*> Ts;                          // индексированы по текстовому имени ТС
    QHash <int, class Ts*> TsIndexed;                       // индексированы по индексу ТС
    QHash <int, class Ts*> TsByIndxTsName;                  // индексированы по индексу имени
    QVector <class Ts*> TsSorted;                           // отсортированы по имени

    // таблицы ТУ по станции
    QHash <QString, class Tu*> Tu;                          // индексированы по текстовому имени ТУ
    QHash <int    , class Tu*> TuByIJ;                      // индексированы по IJ
    QVector <class Tu*> TuSorted;                           // отсортированы по имени

    QHash <QString, class Otu*> Otu;                        // ОТУ по станции, отсортированне по имени
    QHash <int, class Otu*> OtuByNo;                        // ОТУ по станции, отсортированне по номеру ОТУ

    QVector <class ShapeId*> formList;                      // список классов-идентификаторов форм


    bool IsTsPresent(int i) { return i>=0 && i<MaxModule && mts[i]; }
    bool IsTuPresent(int i) { return i>=0 && i<MaxModule && mtu[i]; }
    class Svtf * GetSvtfByName(QString& name);              // найти светофор по имени; используется при парсинге справочника маршрутов
    class Strl * GetStrlByName(QString& name, int& no);     // найти стрелку по имени[возможно, индексированному]
    class Rc   * GetRcByName  (QString& name);              // найти РЦ по имени[возможно, индексированному]
    class Tu   * GetTuByName  (QString& name);              // поиск ТУ по имени с возможностью индексации станции ЧО[#3], ЧО[Вад]

    void  ClearRcAndRouteInfo();                            // очистка информации о состоянии РЦ и маршрутов перед приемом данных из потока
    bool  IsUndefinedTsPresrnt();                           // проверка наличия в матрице ТС сигналов, не описанных в БД
    bool  IsKpOk() { return stsKpOk; }                      // проверка работоспособности КП
    bool IsOrientEvnOdd() { return orient.indexOf("ЧН") >= 0; }

    bool IsTsChanged();                                     // проверка были ли изменения ТС с прошлого цикла опроса
    bool IsLinkOk();                                        // состояние связи в последнем циклн опроса ОК
    bool IsLinkStatusChanged();                             // проверка изменения состояния связи со станцией с пред.цикла опроса

    QString& TypeEC() { return typeEC; }

// вычисление переменной - через обработку сигнала в слоте
public slots:
   void GetValue(QString& name, int& ret);                    // вычисление переменной в выражении формата ИМЯ_ТС[ИМЯ_ИЛИ_#НОМЕР_СТАНЦИИ]

// закрытые члены
private:
   QHash <int, class Rc  *> allrc;                         // РЦ        станции, индексированные по номеру объекта
   QHash <int, class Svtf*> allsvtf;                       // Стрелки   станции, индексированные по номеру объекта
   QHash <int, class Strl*> allstrl;                       // Светофоры станции, индексированные по номеру объекта
   QHash <int, class Route*>routes;                        // маршруты на станции, индексированные по номеру маршрута на станции

    class KrugInfo* krug;                                  // класс круга
    int     no;                                             // номер
    QString noext;                                          // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]
    QString name;
    int     ras;                                            // номер станции связи
    int     addr;                                           // линейный адрес
    int     krugId;                                         // номер круга из БД
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
    QString typeEC;                                         // тип ЭЦ

    //	АДКСЦБ  EBILOCK АПКДК  РПЦДИАЛОГ  МПЦДИАЛОГ  МТУ=1(1)  МТС=19(2-20)  АБТЦМ
    bool    mpcEbilock;                                     // конфигурация с Ebilock950, а также ЭЦ ЕМ, РПЦ/МПЦ МПК + rpcMpcMPK
    bool    rpcMpcMPK;                                      // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool    rpcDialog;                                      // конфигурация с РПЦ Диалог
    bool    apkdk;                                          // конфигурация с АПКДК
    bool    adkScb;                                         // конфигурация с АДКСЦБ
    bool	abtcm;                                          // конфигурация с АБТЦМ
    bool	dcMpk;                                          // КП - ДЦ МПК

    bool    upokOtu;                                        // конфигурация с УПОК
    bool    bybylogic;                                      // логика удалений (У1,У2)

    int     nEbilockTsLength;                               // длина массива Ebilock из строки конфигурации КП "EBILOCK(80)"

    QBitArray     mts;                                      // пометка модулей ТС
    QBitArray     mtu;                                      // пометка модулей ТУ
    int     mtsCount;                                       // число модулей ТС
    int     mtuCount;                                       // число модулей ТУ
    bool          mvv2present;                              // наличие МВВ2

    // состояние сигналов ТС
    QBitArray tsInverse;                                    // битовый массив инверсии
    QBitArray tsStsRaw;                                     // текущий съем с объекта позиционного состояния сигналов в сыром виде
    QBitArray tsStsPulse;                                   // текущий съем с объекта состояния мигания в сыром виде
    QBitArray tsStsRawPrv;                                  // позиционное состояние на пред.шаге
    QBitArray tsStsPulsePrv;                                // мигание на пред.шаге
    QBitArray tsSts;                                        // обработанный массив ТС

    bool stsKpOk;                                           // состояние КП = ОК
    bool stsLinkOkPrv;                                      // связь ОК в пред.цикле опроса

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

    bool    alarmATU;                                       // признак ошибки АТУ (контроль АТУ для старых КП или состояние в блоки диагностики)

    int     errorLockLogicCount;                            // число актуальных ошибок логического контроля
    bool    stsFrmMntrErrorLockMsgPresent;					// наличие ЗЦ.ОШБ в базовом удаленном АРМ
    bool    stsFrmMntrTsExpired;							// ТС устарели в базовом удаленном АРМ

    class SysInfo * mainSysInfo;                            // блок сист.информации основного  БМ
    class SysInfo * rsrvSysInfo;                            // блок сист.информации резервного БМ
    time_t  tSpokSnd;                                       // время передачи данных в СПОК
    time_t  tSpokRcv;                                       // время приема данных от СПОК

    QDateTime lastAcceptedTsTime;                           // засечка последнего приема данных

    // можно объявить экземпляр класса DStDataFromMonitor, чтобы хранить тут сформированные или полученные данные потока
    // это можно было бы сделать, чтобы избежать полного разбора потока при приеме, просто скопировав данные (наложив шаблон класса)
    // не факт, что игра стоит свеч, так как усложняется определение состояний

    int realStreamTsLength;									// Реальная длина данных ТС в блоке DtFrmMnt из расчета 1 бит на сигнал (192, 384, 512...)
    QString   buf;                                          // строка сообщений

    // данные по перегонам от систем ДК
    QBitArray dirInputDk;                                   // cостояние перегонов от систем ДК в отдельном буфере
    QDateTime lastTimeReceiveDk;                            // время последнего приема сигналов ДК
    int       lenDk;                                        // длина последних принятых данных ДК
    int       offsetDk;                                     // смещение данных ДК в массивах

    QVector<class AbtcmInfo    *> abtcms;                   // описание подключений АБТЦМ
    QVector<class rpcDialogInfo*> rpcDialogs;               // описание подключений РПЦ Диалог
    QVector<class ecMpkInfo    *> ecMpks;                   // описание подключений ЭЦ-МПК

    // закрытые функции
    bool parseNames (QString& srcname, Station*& st, QString& name); // разбор индексированных имен ТУ/ТС
    bool TestBit (QBitArray& bits, int index);              // проверка бита в битовом массиве
    void ParseExtForms();                                   // разбор доп.форм
    void ParseConfigKP2007(Logger& logger);                 // разбор строки конфигурации КП станции
    void ParseMT (bool tu=false);                           // разбор описания модулей МТУ, МТС    
    void ParseTuEclusion();                                 // явное опсание исключений ТУ для разных режимов управления
    void GetVirtualTs();                                    // вычисление виртуальных ТС по формулам
    void CheckTs();                                         // проверка ТС на достоверность (диагональ, "лишние" ТС)
    void CheckMode();                                       // отследить режимы управлеия станцией
    void CheckK7();                                         // отследить состояние К7
    void CheckKp();                                         // проверка работоспособности КП
};

// класс идентификации формы станции (имя формы, ID кнопки)
// в общем случае станция может иметь несколько форм для представления схемы
// чаще всего - одну форму, иногда - ни одной
class ShapeId
{
    friend class ShapeSet;
public:
    ShapeId(Station * st, QString& name, int radioid)
    {
        this->st = st;
        this->radioid = radioid;
        this->name = name;
        fileName = name + ".shp";
        set = nullptr;
    }
    Station * St() { return st; }
    QString Name() { return name; }
    class ShapeSet * Set() { return set; }

private:
    Station * st;                                           // станция-собственник
    int radioid;                                            // кнопка
    QString name;                                           // наименование формы без расширения
    QString fileName;                                       // наименование формы c расширением
    class ShapeSet * set;                                   // контейнер примитивов
    class ShapeChild * widget;                              // окно отображения
};

#endif // STATION_H
