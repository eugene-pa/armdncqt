#include "sprbase.h"
#include "station.h"
#include "ts.h"
#include "tu.h"
#include "properties.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"


// Файл содержит реализацию классов IdentityType, Property, Method

// IdentityType
// ------------------------------------------------------------------------------------------------------------------------
// конструктор
IdentityType::IdentityType (QSqlQuery& query, Logger& logger)
{
    try
    {
          name      = query.value("Name"    ).toString();   // номер станции
          obtype    = query.value("ObjType" ).toString();   // РЦ/СВТФ/СТРЛ
          proptype  = query.value("PropType").toString();   // ТУ/ТС
          regex.setPattern(query.value("Regex").toString());// шаблон
          description= query.value("Description").toString();

          // класс сформирован, обавляем в соответствующий список шаблонов имен РЦ/СВТФ/СТРЛ
          Rc  ::AddTemplate(this);
          Svtf::AddTemplate(this);
          Strl::AddTemplate(this);
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе IdentityType");
    }
}

// чтение БД описателей свойств и методов
bool IdentityType::ReadBd (QString& dbpath, Logger& logger)
{
    logger.log(QString("Чтение таблицы [Properties] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Properties]");

    try
    {
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
        if (dbSql.open())
        {
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    new IdentityType(query, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции IdentityType::ReadBd");
        return false;
    }

    return true;
}




// Property
// ------------------------------------------------------------------------------------------------------------------------

QString Property::empty;                                    // статическая пустая строка

Property::Property (QString name, std::unordered_map<std::string, class IdentityType *> hash, Logger& logger)
{
    if (hash.count(name.toStdString()))
    {
        type = hash[name.toStdString()];
        ts = nullptr;
    }
    else
    {
        type = nullptr;
        logger.log(QString("Не определен шаблон для свойства РЦ '%1'").arg(name));
    }
}

// имя ТС
QString& Property::NameTs()
{
    return ts == nullptr ? empty : ts->Name();
}

QString& Property::NameTsEx()                               // Ст.имя_станции ТС=имя_тс
{
    return ts == nullptr ? empty : ts->NameEx();
}

// разбор ТС на принадлежность свойству
bool Property::Parse (class Ts * ts, Logger& logger)
{
    bool ret= false;

    if (type != nullptr && type->Regex().match(ts->Name()).hasMatch())
    {
        if (this->ts == nullptr)
        {
            ts->SetParsed();
            this->ts = ts;
            ret= true;
//          logger.log(QString("Определение свойства %1: %2, %3").arg(NameProp()).arg(NameTsEx()).arg(ts->NameEx()));
        }
        else
        {
            logger.log(QString("Переопределение свойства %1: %2, %3").arg(NameProp()).arg(NameTsEx()).arg(ts->NameEx()));
        }
    }
    return ret;
}

// состояние
bool Property::Value()
{
    return Valid() ? ts->Value() : false;
}

// если свойство определено - вернуть описание свойства, ТС и состояние, иначе - пустая строка
QString Property::About()
{
    QString s;
    if (ts != nullptr)
        s = QString("\r\nТС %1: %2 = %3").arg(type->Name()).arg(NameTs()).arg(Value() ? 1 : 0);
    return s;
}


// Method
// ------------------------------------------------------------------------------------------------------------------------
QString Method::empty;                                      // статическая пустая строка

Method::Method (QString name, std::unordered_map<std::string, class IdentityType *> hash, Logger& logger)
{
    if (hash.count(name.toStdString()))
    {
        type = hash[name.toStdString()];
        tu = nullptr;
    }
    else
    {
        type = nullptr;
        logger.log(QString("Не определен шаблон для метода РЦ '%1'").arg(name));
    }
}

// разбор ТУ на принадлежность методу
bool Method::Parse (class Tu * tu, Logger& logger)
{
    bool ret= false;
    if (type != nullptr && type->Regex().match(tu->Name()).hasMatch())
    {
        if (this->tu == nullptr)
        {
            tu->SetParsed();
            this->tu = tu;
            ret= true;
//          logger.log(QString("Привязка метода '%1': %2").arg(NameProp()).arg(NameTuEx()));
        }
        else
        {
            logger.log(QString("Переопределение метода '%1': %2, %3").arg(NameProp()).arg(NameTuEx()).arg(tu->Name()));
        }
    }
    return ret;
}

// имя ТУ
QString& Method::NameTu()
{
    return tu == nullptr ? empty : tu->Name();
}

QString& Method::NameTuEx()                               // Ст.имя_станции ТУ=имя_тс
{
    return tu == nullptr ? empty : tu->NameEx();
}

// если свойство определено - вернуть описание свойства, ТС и состояние, иначе - пустая строка
QString Method::About()
{
    QString s;
    if (tu != nullptr)
        s = QString("\r\nТУ %1: %2").arg(type->Name()).arg(NameTu  ());
    return s;
}
