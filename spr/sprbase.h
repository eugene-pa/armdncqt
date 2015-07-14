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

    int     No  ()  { return no; }                          // номер объекта
    QString& Name()  { return name; }                       // имя объекта (м.б.сложным)
    QString& NameEx();                                      // имя станции, имя объекта
    int Id  ();                                             // уникальный идентификатор с учетом круга
    int IdSt() { return nost; }                             // номер станции
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

    int IdRc   () { return norc  ; }                        // номер РЦ
    int IdSvtf () { return nosvtf; }                        // номер светофора
    int IdStrl () { return nostrl; }                        // номер стрелки

    bool Locked() { return locked; }

    BaseType GetBaseType() { return basetype; }             // тип объекта
    void SetBaseType(BaseType t) { basetype = t; }
    QString& GetBaseName();

    void * Tag() { return tag; }                            // объект пользователя

protected:
    BaseType basetype;                                      // тип объекта
    int     no;                                             // числовой номер/идентификаторр/ключ объекта
    QString name;                                           // имя объекта

    int     nost;                                           // номер станции
    class Station * st;                                     // указатель на класс станции

    int     norc;                                           // номер РЦ или 0
    int     nostrl;                                         // номер СВТФ или 0
    int     nosvtf;                                         // номер СТРЛ или 0

    bool   locked;                                          // объект заблокирован

    QVector <class Ts*> tsList;                             // список всех ТС объекта
    QVector <class Tu*> tuList;                             // список всех ТУ объекта

    class KrugSpr * krug;                                   // указатель на класс круга
    bool    enabled;                                        // включен (не отключен)
    void * tag;                                             // пользовательский объект по анлогии C#
    static QString buf;                                     // статический буфер для формирования сообщений
};

#endif // SPRBASE_H
