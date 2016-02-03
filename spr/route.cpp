#include "route.h"

QHash <int, Route *> Route::routes;                             // маршруты, индексированные по индексу ТС

Route::Route(QSqlQuery& query, Logger& logger) : SprBase()
{
    Q_UNUSED(query)
    bool ret = true;

    svtfBeg = svtfEnd = nullptr;                                // обнуляем указатели
    srcStrlError    = false;                                    // ошибки описания сбрасываем
    srcRcError      = false;
    tuSetError      = false;
    nextRc          = nullptr;                                  // РЦ, следующая  за ограждающим светофором (для логич.контроля)
    fBegEnd         = false;                                    // Признак маршрутного набора - задание НАЧАЛО КОНЕЦ
    bSetStrlBefore  = false;                                    // Признак необходимости перевести стрелки перед дачей команды
    bInOutRetime    = false;
    hangSetModeExpr = false;                                    // вычислитель выражения контроля "зависания" набора маршрута
    prgTsExpr       = false;                                    // вычислитель выражения контроля требуемого направления на перегоне
    zmkExpr         = false;                                    // вычислитель выражения контроля замыкания
    bSetStrlBefore  = false;                                    // Признак необходимости перевести стрелки перед дачей команды (лексема СТРЕЛКИ)

    try
    {
        // Поле Cod должно быть обязательно!
        sts     = PASSIVE;
        id      = query.value("Cod" ).toInt(&ret);              // код маршрута
        nost    = query.value("NoSt").toInt(&ret);              // номер станции
        no      = query.value("No").toInt(&ret);                // номер маршрута на станции
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

        // [STRL] чтение и разбор списка стрелок в заданном положении [Strl]
        srcStrl     = query.value("Strl").toString().trimmed();
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

        // [Rc] чтение и разбор списка РЦ [Rc]
        srcRc = query.value("Rc").toString().trimmed();
        if (srcRc.length() > 0)
        {
            QStringList list = srcRc.split(QRegExp("[\\s,]+"));
            foreach (QString s, list)
            {
                Rc * rc = st->GetRcByName(s);
                if (rc != nullptr)
                    listRc.append(rc);
                else
                {
                    logger.log(QString("Маршрут %1. Ошибка описания РЦ %2 в маршруте: '%3'").arg(NameLog()).arg(s).arg(srcRc));
                    srcRcError = true;
                }
            }
        }

        // [TuBegEnd] - последовательность ТУ установки маршрута
        tuSetError = !ParseTuList (query, "TuBegEnd", srcTuSet, tuSetList, logger);

        // [TuCancel] - последовательность ТУ отмены маршрута
        tuCancelError = !ParseTuList (query, "TuCancel", srcTuCancel, tuCancelList, logger);

        // [CancelModeTu] - последовательность ТУ отмены набора
        tuCancelPendingError = !ParseTuList (query, "CancelModeTu", srcTuCancelPending, tuCancelPendingList, logger);

        // [CancelModeTs] - контроль "зависания" набора маршрута
        hangSetModeError = !ParseExpression(query, "CancelModeTs", srcHangSetMode, hangSetModeExpr, logger);

        // [PrgTsExpr] - выражение для контроля требуемого направления на перегоне
        srcPrgTsError = !ParseExpression(query, "PrgTsExpr", srcPrgTsExpr, prgTsExpr, logger);

        // [PrgTu] - список ТУ разворота перегона
        srcPrgTuError = !ParseTuList (query, "PrgTu", srcPrgTu, tuPeregon, logger);

        // [CodZmk] - делаем подстановку и читаем это поле как символьное с учетом возможности задания формулы
        srcZzmk = query.value("CodZmk").toString();
        int indxTsZmk = srcZzmk.toInt(&ret);
        if (ret)
        {
            // если 0 - нет выражения
            if (indxTsZmk)
            {
                srcZzmk = indxTsZmk == 0 ? "" : st->TsByIndxTsName.contains(indxTsZmk) ? st->TsByIndxTsName[indxTsZmk]->Name() : "";
                if (srcZzmk.length() == 0)
                    logger.log(QString("Маршрут %1. Ошибка описания поля CodZmk в маршруте: '%2'").arg(NameLog()).arg(srcZzmk));
            }
            else
                srcZzmk.clear();
        }
        if (srcZzmk.length() > 0)
        {
            zmkExpr = new BoolExpression(srcZzmk);
            srcZzmkError = !zmkExpr->Valid();
        }

        // [Complex] - перечисление составных маршрутов через + (или пробел)
        // читаем строку, разбор строки выполняется на "втором проходе" после чтения всей таблицы, чтобы не зависить от порядка описания маршрутов в таблице
        // иначе, если составной предшествует элементарному, имеем ошибку неопределенных маршрутов
        // см. static public void CheckComplex(paLog logger)
        srcComplex = query.value("Complex").toString();
        if (srcComplex == "-")
            srcComplex.clear();


        // [Враждебные маршруты] - перечисление через пробел (или запятую)
        // читаем строку, разбор строки выполняется на "втором проходе" после чтения всей таблицы, чтобы не зависить от порядка описания маршрутов в таблице
        // чм. static public void CheckOpponents(paLog logger)
        srcOpponentRoutes = query.value("Враждебные маршруты").toString();
        if (srcOpponentRoutes.length() > 0 && srcOpponentRoutes=="-")
            srcOpponentRoutes.clear();

        // [TimeCloseSvtf] - максим.время перекрытия сигнала
        timeCloseSvtf = query.value("TimeCloseSvtf").toInt(&ret);

        // [Question]
        srcComment = query.value("Question").toString();

        // [Park]
        srcpark = query.value("Park").toString();

        int key = id; // GetKey(krug, Cod);

        routes[key] = this;                                 // добавляем справочник в общий список маршрутов
        st->Allroute()[no] = this;                          // добавляем справочник в список маршрутов станции
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

//    CheckComplex  (krug, logger);                         // обработать списки составных
//    CheckOpponents(krug, logger);                         // обработать списки враждебных

    return ret;
}

QString Route::NameLog()
{
    return QString("Ст.%1. Маршрут №%2").arg(st->Name()).arg(id);
}

// чтение последовательности ТУ, проверка синтаксиса и запись в список
bool Route::ParseTuList (QSqlQuery& query, QString field, QString& src, QVector <Tu*> list, Logger& logger)
{
    bool ret = true;
    src = query.value(field).toString().trimmed();
    if (src=="0")                                           // страхуемся от случаев, когда вместо пустой строки стоит 0
        src.clear();
    if (src.length())
    {
        QStringList tulist = src.split(QRegExp("[\\s,]+"));
        foreach (QString s, tulist)
        {
            if (s.toUpper() == "СТРЕЛКИ")
            {
                bSetStrlBefore = true;                      // признак индивидуального перевода стртелок перед установкой маршрута
                continue;
            }
            Tu * tu = st->GetTuByName(s);
            if (tu != nullptr)
                list.append(tu);                            // список ТУ установки маршрута
            else
            {
                logger.log(QString("Маршрут %1. Ошибка описания ТУ %2 в маршруте: [%3]='%4'").arg(NameLog()).arg(s).arg(field).arg(src));
                ret = false;
            }
        }
    }
    return ret;
}

// создание вычислителя для обработки формул
bool Route::ParseExpression(QSqlQuery& query, QString field, QString& src, BoolExpression *& expr, Logger& logger)
{
    bool ret = true;
    expr = nullptr;
    src = query.value(field).toString().trimmed();
    if (src.length())
    {
        expr = new BoolExpression(src);
        if (expr->Valid())
            QObject::connect(expr, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        else
        {
            ret = false;
            logger.log(QString("Маршрут %1. Ошибка выражения в поле %2 '%3': %4").arg(NameLog()).arg(field).arg(expr->Source()).arg(expr->ErrorText()));
        }
    }
    return true;
}

//{
//    LogicalExpression expr = null;
//    if (src.Length > 0)
//    {
//        expr = new LogicalExpression(src, SprSt.GetVar, null);
//        if (!expr.Valid)
//            Log(string.Format("{0}. [{1}]: {2}", NameEx, field, expr.ErrorTextFull), logger);
//    }
//    return expr;
//}
