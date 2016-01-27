#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "sprbase.h"
#include "../common/logger.h"

#define SafeValue(p) p==nullptr ? false : p->Value()

// класс описания шаблонов свойств
class IdentityType
{
public:
    static bool ReadBd (QString& dbpath, Logger& logger);   // чтение БД
    IdentityType (QSqlQuery& query, Logger& logger);        // конструктор
    QString& Name()     { return name;      }               // имя свойства
    QString& PropType() { return proptype;  }               // ТУ/ТС
    QString& ObjType () { return obtype;    }               // РЦ/СВТФ/СТРЛ
    QRegularExpression Regex() { return regex; }
private:
    QString obtype;                                         // РЦ/СВТФ/СТРЛ
    QString proptype;                                       // ТУ/ТС
    QString name;                                           // имя свойства
    QRegularExpression regex;                               // шаблон
    QString description;
};

// класс описания свойств
class Property
{
public:
    Property (QString name, QHash<QString, class IdentityType *> hash, Logger& logger);
    bool Valid() { return ts != nullptr; }                  // валидность свойства
    bool Value();                                           // состояние
    QString& NameTs  ();                                    // имя ТС
    QString& NameTsEx();                                    // Ст.имя_станции ТС=имя_тс
    QString& NameProp() { return type->Name(); }
    bool Parse (class Ts * ts, Logger& logger);             // разбор ТС на принадлежность свойству
    class Ts * Ts() { return ts; }                          // указатель на класс ТС свойства или NULL
    void SetTs(class Ts * ptr) { ts = ptr; }                // назначить ТС
private:
static QString empty;                                       // пустая строка

    IdentityType * type;                                    //
    class Ts * ts;                                          // ТС, определяющий свойство
};


// класс описания методов
class Method
{
public:
    Method (QString name, QHash<QString, class IdentityType *> hash, Logger& logger);

    bool Valid() { return tu != nullptr; }                  // валидность свойства
    bool Sts();
    QString& NameTu  ();                                    // имя ТС
    QString& NameTuEx();                                    // Ст.имя_станции ТС=имя_тс
    QString& NameProp() { return type->Name(); }
    bool Parse (class Tu * tu, Logger& logger);             // разбор ТС на принадлежность свойству
    class Tu * Tu() { return tu; }                          // указатель на класс ТС свойства или NULL

private:
    static QString empty;                                       // пустая строка

        IdentityType * type;                                    //
        class Tu * tu;                                          // ТС, определяющий свойство
};


#endif // PROPERTIES_H
