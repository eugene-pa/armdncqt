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
        BaseTs,                                         // = 1
        BaseTu,                                         // = 2
        BaseRc,                                         // = 3
        BaseSvtf,                                       // = 4
        BaseStrl,                                       // = 5
        BaseMax,                                        // = 6
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
};

#endif // SPRBASE_H
