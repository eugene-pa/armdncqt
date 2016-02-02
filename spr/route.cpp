#include "route.h"

QHash <int, Route *> Route::routes;                             // маршруты, индексированные по индексу ТС

Route::Route(QSqlQuery& query, Logger& logger) : SprBase()
{
    Q_UNUSED(query)
    bool ret = true;

    svtfBeg = svtfEnd = nullptr;                                // обнуляем указатели
    nextRc = nullptr;
    srcStrlError = srcRcError = false;                          // ошибки описания сбрасываем
    fBegEnd = false;                                            // Признак маршрутного набора - задание НАЧАЛО КОНЕЦ
    bSetStrlBefore = false;                                     // Признак необходимости перевести стрелки перед дачей команды


    try
    {
        // Поле Cod должно быть обязательно!
        sts     = PASSIVE;
        id      = query.value("Cod" ).toInt(&ret);              // код маршрута
        nost    = query.value("NoSt").toInt(&ret);              // номер станции
        st      = Station::GetById(nost);                       // справочник
        if (st==nullptr)
        {
            logger.log(QString("Маршрут %1 - не найден справочник станции для маршрута. Игнорируем маршрут").arg(nost));
            // поля остаются неинициализированными!
            return;
        }
        text    = query.value("Text").toString().trimmed();     // описание
        QString tmp = query.value("Dir").toString().trimmed();  // направление Ч/Н
        dir     = tmp != "Ч";                                   // false = Ч, true = Н
        path    = query.value("Path").toString().trimmed();     // основной путь приема/отправления; в маршрутах передачи путь на который отправляется поезд
        pathFrom= query.value("PathFrom").toString().trimmed(); // путь отправления в маршрутах передачи с пути на путь

        typeName= query.value("Type").toString().trimmed();     // имя типа
        type    = (ROTE_TYPE)query.value("codetype").toInt(&ret);// чтение псевдонима codetype РАБОТАЕТ!

        tmp     = query.value("Svtf").toString().trimmed();     // имя светофора, ограждающего начало маршрута
        if (tmp.length() > 0)
            svtfBeg = st->GetSvtfByName(tmp);

        tmp     = query.value("SvtfEnd").toString().trimmed();  // необязательное имя светофора, ограждающего конец маршрута
        if (tmp.length() > 0)
            svtfEnd = st->GetSvtfByName(tmp);

        // чтение и разбор списка стрелок в заданном положении [Strl]
        srcStrl     = query.value("Strl").toString().trimmed(); // стрелки
        if (srcStrl.length() > 0)
        {
            QStringList list = srcStrl.split(QRegExp("[\\s,]+"));
            for (int i=0; i<list.length(); i++)
            {
                // в общем случае нужно уметь задавать индексированные описания, например: 221+[Минводы]
                LinkedStrl * lnk = new LinkedStrl(st, list[i]);
                if (lnk->strl != nullptr)
                    listStrl.append(lnk);
                else
                {
                    logger.log(QString("Маршрут %1. Ошибка описания стрелки %2 в маршруте: '%3'").arg(NameLog()).arg(list[i]).arg(srcStrl));
                    srcStrlError = true;
                }
            }
        }

        // чтение и разбор списка РЦ [Rc]
//        RcSrc = SafeGetStr(rdr, "Rc");
//        string[] arRc = RcSrc.Split(new char[] { ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
//        foreach (string s in arRc)
//        {
//            RcInfo rc = SprSt.GetRcByNameEx(s);
//            if (rc != null)
//                RcList.Add(rc);
//            else
//            {
//                Log(string.Format("{0}. Ошибка описания РЦ {1} в маршруте: '{2}'", NameEx, s, RcSrc), logger);
//                RcSrcError = true;
//            }
//        }

        bInOutRetime = false;
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

QString Route::NameLog()
{
    return QString("Ст.%1. Маршрут №%2").arg(st->Name()).arg(id);
}
