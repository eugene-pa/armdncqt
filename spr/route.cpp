#include "route.h"

QHash <int, Route *> Route::routes;                     // маршруты, индексированные по индексу ТС

Route::Route()
{

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
