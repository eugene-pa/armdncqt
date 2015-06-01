#include "sprbase.h"
#include "station.h"
#include "ts.h"
#include "properties.h"

#include "rc.h"
#include "svtf.h"
#include "strl.h"

QString Property::empty;                                    // статическая пустая строка

Property::Property (QString name, QHash<QString, class IdentityType *> hash, Logger& logger)
{
    if (hash.contains(name))
    {
        type = hash[name];
        ts = nullptr;
    }
    else
    {
        logger.log(QString("Не определен шаблон для свойства РЦ '%1'").arg(name));
    }
}

QString& Property::NameTsEx()                               // Ст.имя_станции ТС=имя_тс
{
    return ts == nullptr ? empty : ts->NameEx();
}

// разбор ТС на принадлежность свойству
bool Property::Parse (class Ts * ts, Logger& logger)
{
    bool ret= false;
    if (this->ts == nullptr)
    {
        if (type->Regex().match(ts->Name()).hasMatch())
            this->ts = ts;
    }
    else
    {
        logger.log(QString("Переопределение свойства %1: %2, %3").arg(NameProp()).arg(NameTsEx()).arg(ts->NameEx()));
    }
    return ret;
}

// чтение БД описателей свойств и методов
bool IdentityType::ReadBd (QString& dbpath, Logger& logger)
{
    logger.log(QString("Чтение таблицы [Properties] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Properties]");

    try
    {
        bool exist = false;
        QSqlDatabase dbSql = (exist = QSqlDatabase::contains(dbpath)) ? QSqlDatabase::database(dbpath) :
                                                                        QSqlDatabase::addDatabase("QSQLITE", dbpath);
        if (!exist)
            dbSql.setDatabaseName(dbpath);
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

          // класс сформирован, обавляем в соответствующий список шаблоно имен РЦ/СВТФ/СТРЛ
          Rc  ::AddTemplate(this);
          Svtf::AddTemplate(this);
          Strl::AddTemplate(this);
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе IdentityType");
    }
}


