#include <QVariant>
#include "sprbase.h"
#include "station.h"
#include "ts.h"
#include "tu.h"
#include "../common/boolexpression.h"
#include "rc.h"
#include "strl.h"
#include "svtf.h"


Tu::Tu(QSqlQuery& query, Logger& logger)
{
    bool ret;
    try
    {
        iname   = query.value("Cod"     ).toInt(&ret);      // код имени ( в C# было "TS.Cod")
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Tu");
    }


}

Tu::~Tu()
{

}

// чтение БД
bool Tu::ReadBd (QString& dbpath, Logger& logger)
{
    logger.log(QString("Чтение таблицы [ТU] БД %1").arg(dbpath));

    try
    {
        bool exist = false;
        QSqlDatabase dbSql = (exist = QSqlDatabase::contains(dbpath)) ? QSqlDatabase::database(dbpath) :
                                                                        QSqlDatabase::addDatabase("QSQLITE", dbpath);
        if (!exist)
            dbSql.setDatabaseName(dbpath);

        if (dbSql.open())
        {

            // ПРОБЛЕМА: если в таблице нет имени с кродом 0, запрос ниже выдает пустой набор данных
            // 1. пытаемся прочитать имя с нулевым индексом
            QSqlQuery queryTu0(dbSql);
            QString sql("SELECT * FROM TU_Name WHERE Cod=0");
            if (queryTu0.exec (sql) && !queryTu0.next())
            {
                logger.log("В таблице TU_Name отсутствует имя с нулевым индексом. Таблица TU не может быть корректно обработана");
            }


            sql = "SELECT *, c.NameTU, p.NameTU, e.NameTU, s.NameTu FROM ((((TU INNER JOIN TU_Name AS c ON TU.Cod = c.Cod) INNER JOIN TU_Name AS p ON TU.Prolog = p.Cod) INNER JOIN TU_Name AS e ON TU.Epilog = e.Cod) INNER JOIN TU_Name AS s ON TU.Polus  = s.Cod)  ORDER BY NOST,[Module],[I],[J]";

            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    new Tu(query, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Tu::ReadBd");
        return false;
    }

//  logger.log("Сортировка списков ТУ");
//  Station::sortTu();

    return true;

}

// Имя станции и ТС
QString& Tu::NameEx()
{
    return buf = QString("%1 ТУ='%2'").arg(StMessage()).arg(name);
}

int Tu::CompareByNames(const void* p1,const void* p2)
{
    return ((Tu*)p1)->Name() < ((Tu*)p2)->Name();
}
