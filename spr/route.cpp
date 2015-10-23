#include "route.h"

QHash <int, Route *> Route::routes;                     // маршруты, индексированные по индексу ТС

Route::Route(QSqlQuery& query, Logger& logger)
{
    bool ret = true;
    try
    {
        //id =
    }
    catch(...)
    {
        ret = false;
        logger.log("Исключение в конструкторе Route");
    }
}

Route::~Route()
{

}

// получить справочник по уникальному ключу(номеру) маршрута
Route * Route::GetById(int id)
{
    return routes.contains(id) ? routes[id] : nullptr;
}

// получить справочник по уникальному ключу(номеру) маршрута
Route * Route::GetByNo(int no, Station* st)
{
    return st ? st->GetRouteByNo(no) : nullptr;
}

// чтение БД
bool Route::ReadBd (QString& dbpath, Logger& logger)
{
    bool ret = true;
    logger.log(QString("Чтение таблиц маршрутов RouteSrc,RouteTypes,TypeOfRout БД %1").arg(dbpath));
    QString sql("SELECT *, t.Cod as codetype FROM (RouteSrc INNER JOIN RouteTypes ON RouteSrc.IdType = RouteTypes.IdType) INNER JOIN TypeOfRout as t ON RouteSrc.Type = t.Typ");

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
                    new Route(query, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Route::ReadBd");
        ret = false;
    }
    return ret;
}
