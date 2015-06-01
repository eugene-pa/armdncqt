#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "sprbase.h"
#include "../common/logger.h"


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
//    Property (IdentityType& type)
//    {
//        this->type = &type;
//        ts = nullptr;
//    }
    bool Valid() { return ts != nullptr; }                  // валидность свойства
    bool Sts();
    QString& NameTs  ();                                    // имя ТС
    QString& NameTsEx();                                    // Ст.имя_станции ТС=имя_тс
    QString& NameProp() { return type->Name(); }
    bool Parse (class Ts * ts, Logger& logger);             // разбор ТС на принадлежность свойству
    class Ts * Ts() { return ts; }                          // указатель на класс ТС свойства или NULL

private:
static QString empty;                                       // пустая строка

    IdentityType * type;                                    //
    class Ts * ts;                                          // ТС, определяющий свойство
};



#endif // PROPERTIES_H
