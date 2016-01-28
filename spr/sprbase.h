#ifndef SPRBASE_H
#define SPRBASE_H

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QHash>
#include <QVector>
#include <QStack>

#include "../common/logger.h"

QSqlDatabase GetSqliteBd(QString& dbpath);                  // глобальная функция получения объекта БД по имени БД

class SprBase                                               // базовый класс справочника
{
public:
    SprBase();
    ~SprBase();

    enum Direction
    {
        Even    = 0,
        Odd     = 1,
    };

    enum BaseType
    {
        BaseUnknown = 0,
        BaseTs,                                             // = 1
        BaseTu,                                             // = 2
        BaseRc,                                             // = 3
        BaseSvtf,                                           // = 4
        BaseStrl,                                           // = 5
        BaseMax,                                            // = 6
    };

    enum UniType
    {
        Unknown     = 0,
        UniRc       = 1,                                    // РЦ
        UniStrl     = 2,                                    // стрелка
        UniSvtfM    = 3,                                    // маневровый светофор
        UniSvtf     = 4,                                    // Светофор станционный поездной (входной, выходной, маршрутный)
        UniSvtfZ    = 5,	                                // Заградительный светофор
        UniSvtfPrh  = 6,	                                // Проходной светофор
        UniPrzd     = 7,	                                // Переезд
        UniPower    = 8,	                                // Устройства электропитания
        UniStatus   = 9,	                                // Индикатор состояния устройства
        UniBreakstop= 10,	                                // Тормозной упор (УТС)
        UniSwitch    =11,	                                // Стрелочный коммутатор
        UniController= 101,	                                // Контроллер
        UniHub      = 102,	                                // Концентратор
        UniPC       = 103,	                                // Промышленный компьютер
        UniLink     = 104,	                                // Линия связи
        UniSys      = 105,	                                // Система в целом
    };

    // унифицированные типы состояний общие
    enum UniStatus
    {
        StsExpired  = 0,                                    // Неопределенное состояние (отсутствие ТС)
        StsError    = 1,                                    // Неопределенное состояние (несоответствие ТС)
        StsLocked   = 2,                                    // Объект законсервирован
    };

    // унифицированные типы состояний  унифицированного объекта РЦ (UniRc)
    enum UniStatusRc
    {
        StsFreeUnlocked = 3,                                // РЦ свободна, не замкнута
        StsFreeLocked   = 4,                                // РЦ свободна, замкнута
        StsFreeLockedIr = 5,                                // РЦ свободна, замкнута, искусственно размыкается
        StsBusyUnlocked = 6,                                // РЦ занята, не замкнута
        StsBusyLocked   = 7,                                // РЦ занята, замкнута
        StsBusyLockedIr = 8,                                // РЦ занята, замкнута, искусственно размыкается
    };

    // унифицированные типы состояний  унифицированного объекта стрелка (UniStrl)
    enum UniStatusStrl
    {
        StsPlusFreeUnlocked     = 3,                        // Стрелка в плюсовом положении, свободна, не замкнута
        StsPlusFreeLocked       = 4,                        // Стрелка в плюсовом положении, свободна, замкнута
        StsPlusFreeLockedIr     = 5,                        // Стрелка в плюсовом положении, свободна, замкнута, искусственно размыкается
        StsPlusBusyUnlocked     = 6,                        // Стрелка в плюсовом положении, занята, не замкнута
        StsPlusBusyLocked       = 7,                        // Стрелка в плюсовом положении, занята, замкнута
        StsPlusBusyLockedIr     = 8,                        // Стрелка в плюсовом положении, занята, замкнута, искусственно размыкается

        StsMinusFreeUnlocked    = 9,                        // Стрелка в минусовом положении, свободна, не замкнута
        StsMinusFreeLocked      = 10,                       // Стрелка в минусовом положении, свободна, замкнута
        StsMinusFreeLockedIr    = 11,                       // Стрелка в минусовом положении, свободна, замкнута, искусственно размыкается
        StsMinusBusyUnlocked    = 12,                       // Стрелка в минусовом положении, занята, не замкнута
        StsMinusBusyLocked      = 13,                       // Стрелка в минусовом положении, занята, замкнута
        StsMinusBusyLockedIr    = 14,                       // Стрелка в минусовом положении, занята, замкнута, искусственно размыкается

        StsErrorFreeUnlocked    = 15,                       // Стрелка потеряла контроль, свободна, не замкнута
        StsErrorFreeLocked      = 16,                       // Стрелка потеряла контроль, свободна, замкнута
        StsErrorBusyUnlocked    = 17,                       // Стрелка потеряла контроль, занята, не замкнута
        StsErrorBusyLocked      = 18,                       // Стрелка потеряла контроль, занята, замкнута
        StsErrorFreeLockedIr    = 19,                       // Стрелка потеряла контроль, свободна, замкнута, искусственно размыкается
        StsErrorBusyLockedIr    = 20,                       // Стрелка потеряла контроль, занята, замкнута, искусственно размыкается

        StsUnknownFreeUnlocked  = 21,                       // Стрелка имеет контроль, но ее положение неизвестно, свободна, не замкнута
    };

    // унифицированные типы состояний  унифицированного объекта маневровый светофор (UniSvtfM)
    enum UniStatusSvtfM
    {
        StsClosedM      = 3,                                // Запрет маневров (синий огонь)
        StsOpenM        = 4,                                // Маневровое показание (белый огонь)
        StsQuick        = 5,                                // Ускоренные маневры
        StsFailM        = 6,                                // Отказ на светофоре
    };


    // унифицированные типы состояний  унифицированного объекта Светофор станционный поездной (входной, выходной, маршрутный) (UniSvtf)
    enum UniStatusSvtf
    {
        StsOpen         = 3,                                // Один зеленый огонь (открыт)
        StsGreen1Blink  = 4,                                // Один зеленый мигающий огонь
        StsYellow1      = 5,                                // Один желтый огонь
        StsYellow1Blink = 6,                                // Один желтый мигающий огонь
        StsYellow2      = 7,                                // Два желтых огня
        StsYellow2Blink1= 8,                                // Два желтых огня, верхний мигающий
        StsYellow3      = 9,                                // Три желтых огня
        StsGreen1BlinkYellow1 = 10,                         // Один зеленый мигающий и один желтый огни
        StsGreen1White1 = 11,                               // Один зелёный огонь, один белый огонь
        StsYellow11White1 = 12,                             // Один жёлтый огонь, один белый огонь
        StsGreen2       = 13,                               // Два зелёных огня
        StsYellow11BlinkWhite1 = 14,                        // Один жёлтый мигающий и один белый огни
        StsClosed       = 15,                               // Один красный огонь (закрыт)
        StsWhite1       = 16,                               // Один белый (маневровое показание)
        StsWhite2       = 17,                               // Два белых (ускоренные маневры)
        StsPrigl        = 18,                               // Один белый мигающий и красный огни (пригласительный)
        StsFail         = 19,                               // Отказ на светофоре
        StsGreen1Yellow1= 20,                               // Один зеленый и один желтый огни
    };

    // унифицированные типы состояний  унифицированного объекта ПЕРЕЕЗД (UniPrzd)
    enum UniStatusPereezd
    {
        StsOpenPrzd     = 3,                                // Переезд открыт
        StsOpenAndIzv   = 4,                                // Переезд открыт и включено извещение
        StsClosedPrzd   = 5,                                // Переезд закрыт
        StsClosedBarrierOn = 6,                             // Переезд закрыт и шлагбаум закрыт
        StsClosedUzpOn  = 7,                                // Переезд закрыт, шлагбаум закрыт и УЗП поднят
    };

    static QVector<QString> BaseNames;                      // имена типов

    // нужно более четко определиться с No и Id. Можно так: No - номер из БД, Id - ключ с учетом круга,
    // однако, в коде уже используется No как ключ, например, в конструкторе Rc: no = tuts->IdRc();
    int key (int no) { return (krugno << 16) | no; }        // получить идентификатор объекта по номеру с учетом круга

    // отдельные функции для номера объекта и уникального идентификатора
    int No     () { return no;     }                        // номер объекта из БД
    int NoSt   () { return nost;   }                        // номер станции
    int NoRc   () { return norc;   }                        // no РЦ
    int NoSvtf () { return nosvtf; }                        // no светофора
    int NoStrl () { return nostrl; }                        // no стрелки

    int Id     () { return key (no);     }                  // уникальный идентификатор с учетом круга
    int IdSt   () { return key (nost);   }                  // ID станции
    int IdRc   () { return key (norc  ); }                  // ID РЦ
    int IdSvtf () { return key (nosvtf); }                  // ID светофора
    int IdStrl () { return key (nostrl); }                  // ID стрелки

    QString& Name()  { return name; }                       // имя объекта (м.б.сложным)
    QString& NameEx();                                      // имя станции, имя объекта
    class Station * St() { return  st; }
//  class Krug * Krug() { return krug; }
    QString& StationName();                                 // имя станции
    QString& StMessage()                                    // сообщение формата "Ст.Овечка"
        { return buf = QString("Ст.%1").arg(StationName()); }

    virtual QString ToString()                              // сообщение формата "(#номер_объекта) имя_объекта"
        { return QString(" (#%1)").arg(no); }
    virtual QString About() { return "Класс Spr"; }         // коротко о классе
    virtual QString ObjectType()  { return ""; }            // тип объекта (РЦ/СТРЕЛКА/СВЕТОФОР)
    virtual UniType GetUniType()  { return Unknown;}        // виртуальная функци, возвращает тип объекта в терминах универсального протокола
    int GetUniSts() { return uniSts; }

    bool Enabled() { return enabled; }                      // включен (не отключен)
    void Enable(bool s = true) { enabled = s; }             // включить

    bool Locked() { return locked; }

    BaseType GetBaseType() { return basetype; }             // тип объекта
    void SetBaseType(BaseType t) { basetype = t; }
    QString& GetBaseName();

    void * Tag() { return tag; }                            // объект пользователя

protected:
    BaseType basetype;                                      // тип объекта
    int     krugno;                                         // номер круга
    int     no;                                             // числовой номер/идентификаторр/ключ объекта
    QString name;                                           // имя объекта

    int     nost;                                           // номер станции
    class Station * st;                                     // указатель на класс станции

    int     norc;                                           // номер РЦ   из БД или 0
    int     nostrl;                                         // номер СВТФ из БД или 0
    int     nosvtf;                                         // номер СТРЛ из БД или 0

    bool   locked;                                          // объект заблокирован

    QVector <class Ts*> tsList;                             // список всех ТС объекта
    QVector <class Tu*> tuList;                             // список всех ТУ объекта

    class KrugSpr * krug;                                   // указатель на класс круга
    bool    enabled;                                        // включен (не отключен)
    void * tag;                                             // пользовательский объект по анлогии C#
    static QString buf;                                     // статический буфер для формирования сообщений

    UniType  unitype;                                       // унифицированный тип по классификации СТДМ
    int      uniSts;                                        // унифицированное состояние
    bool     uniStsChanged;                                 // пометка изменения состояния в терминах унифицированного состояния

};

#endif // SPRBASE_H
