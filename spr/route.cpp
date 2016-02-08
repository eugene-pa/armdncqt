#include "route.h"
#include "krug.h"

QHash <int, Route *> Route::routes;                             // маршруты, индексированные по индексу ТС

Route::Route(QSqlQuery& query, KrugInfo* krug, Logger& logger) : SprBase()
{
    Q_UNUSED(query)
    bool ret = true;

    this->krug = krug;

    svtfBeg = svtfEnd = nullptr;                                // обнуляем указатели
    srcStrlError    = false;                                    // ошибки описания сбрасываем
    srcRcError      = false;
    tuSetError      = false;
    srcComplexError = false;
    srcOpponentError= false;
    srcZzmkError    = false;
    bSetStrlBefore  = false;                                    // Признак необходимости перевести стрелки перед дачей команды
    bInOutRetime    = false;
    fBegEnd         = false;                                    // Признак маршрутного набора - задание НАЧАЛО КОНЕЦ
    bSetStrlBefore  = false;                                    // Признак необходимости перевести стрелки перед дачей команды (лексема СТРЕЛКИ)

    nextRc          = nullptr;                                  // РЦ, следующая  за ограждающим светофором (для логич.контроля)
    hangSetModeExpr = nullptr;                                  // вычислитель выражения контроля "зависания" набора маршрута
    prgTsExpr       = nullptr;                                  // вычислитель выражения контроля требуемого направления на перегоне
    zmkExpr         = nullptr;                                  // вычислитель выражения контроля замыкания


    try
    {
        // Поле Cod должно быть обязательно!
        sts     = PASSIVE;
        no      = query.value("Cod" ).toInt(&ret);              // код маршрута-ключ
        nost    = query.value("NoSt").toInt(&ret);              // номер станции
        relNo   = query.value("No").toInt(&ret);                // номер маршрута на станции
        st      = Station::GetById(nost);                       // справочник
        if (st==nullptr)
        {
            logger.log(QString("Маршрут %1 - не найден справочник станции для маршрута. Игнорируем маршрут").arg(nost));
            // поля остаются неинициализированными!
            return;
        }
        text    = query.value("Text").toString().trimmed();     // описание
        name    = text;                                         // возможно, нужно более информативное имя

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
                    logger.log(QString("Маршрут %1. Ошибка описания стрелки %2 в маршруте: '%3'").arg(nameLog()).arg(list[i]).arg(srcStrl));
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
                    logger.log(QString("Маршрут %1. Ошибка описания РЦ %2 в маршруте: '%3'").arg(nameLog()).arg(s).arg(srcRc));
                    srcRcError = true;
                }
            }
        }

        // [TuBegEnd] - последовательность ТУ установки маршрута
        tuSetError = !parseTuList (query, "TuBegEnd", srcTuSet, tuSetList, logger);

        // [TuCancel] - последовательность ТУ отмены маршрута
        tuCancelError = !parseTuList (query, "TuCancel", srcTuCancel, tuCancelList, logger);

        // [CancelModeTu] - последовательность ТУ отмены набора
        tuCancelPendingError = !parseTuList (query, "CancelModeTu", srcTuCancelPending, tuCancelPendingList, logger);

        // [CancelModeTs] - контроль "зависания" набора маршрута
        hangSetModeError = !parseExpression(query, "CancelModeTs", srcHangSetMode, hangSetModeExpr, logger);

        // [PrgTsExpr] - выражение для контроля требуемого направления на перегоне
        srcPrgTsError = !parseExpression(query, "PrgTsExpr", srcPrgTsExpr, prgTsExpr, logger);

        // [PrgTu] - список ТУ разворота перегона
        srcPrgTuError = !parseTuList (query, "PrgTu", srcPrgTu, tuPeregon, logger);

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
                    logger.log(QString("Маршрут %1. Ошибка описания поля CodZmk в маршруте: '%2'").arg(nameLog()).arg(srcZzmk));
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


        // ПРОБЛЕМА: при импорте/экспорте пробел в имени поля "Враждебные маршруты" заменяется подчеркиванием!
        // [Враждебные маршруты] - перечисление через пробел (или запятую)
        // читаем строку, разбор строки выполняется на "втором проходе" после чтения всей таблицы, чтобы не зависить от порядка описания маршрутов в таблице
        // чм. static public void CheckOpponents(paLog logger)
        srcOpponentRoutes = query.value("Враждебные_маршруты").toString();
        if (srcOpponentRoutes.length() > 0 && srcOpponentRoutes=="-")
            srcOpponentRoutes.clear();

        // [TimeCloseSvtf] - максим.время перекрытия сигнала
        timeCloseSvtf = query.value("TimeCloseSvtf").toInt(&ret);

        // [Question]
        srcComment = query.value("Question").toString();

        // [Park]
        srcpark = query.value("Park").toString();

        routes[Id()] = this;                                // добавляем справочник в общий список маршрутов
        st->Allroute()[relNo] = this;
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
Route * Route::GetById(int no, KrugInfo * krug)
{
    int id = krug==nullptr ? no : krug->key(no);
    return routes.contains(id) ? routes[id] : nullptr;
}

// получить справочник по уникальному ключу(номеру) маршрута
Route * Route::GetByNo(int no, Station* st)
{
    return st ? st->GetRouteByNo(no) : nullptr;
}

// чтение БД
bool Route::ReadBd (QString& dbpath, KrugInfo* krug, Logger& logger)
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
                    new Route(query, krug, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Route::ReadBd");
        ret = false;
    }

    checkComplex  (krug, logger);                         // обработать списки составных
    checkOpponents(krug, logger);                         // обработать списки враждебных

    return ret;
}

// обработать списки составных
void Route::checkComplex  (class KrugInfo* krug, Logger& logger)
{
    foreach (Route * route, routes.values())
    {
        if (krug==nullptr || krug==route->krug)
        {
            route->checkComplex(logger);
        }
    }
}

// обработать списки враждебных
void Route::checkOpponents(class KrugInfo* krug, Logger& logger)
{
    foreach (Route * route, routes.values())
    {
        if (krug==nullptr || krug==route->krug)
        {
            route->checkOpponents(logger);
        }
    }
}

// обработать списки составных  маршрута
bool Route::checkComplex (Logger& logger)
{
    bool ret = true;                                        // сбрасывается при хотя бы одной любой ошибке
    if (srcComplex.length()==0)
        return ret;

    QStringList list = srcComplex.split(QRegExp("[\\s,\\+]+"));

    int indx = 0;
    foreach (QString s, list)
    {
        bool out = false;
        int no = s.toInt(&out);
        if (out)
        {
            Route * route = st->GetRouteByNo(no);
            if (route != nullptr)
            {
                listRoutes.append(route);
                // - светофор либо не задается вообще, либо должен совпадать со светофором первого маршрута в списке
                if (indx == 0)
                {
                    if (svtfBeg!=nullptr && svtfBeg != route->svtfBeg)
                        logger.log(QString("Составной маршрут %1. Светофор первого маршрута в списке не соответствует светофору в маршруте: [Complex]=%2 ").arg(nameLog()).arg(srcComplex));
                    svtfBeg = route->svtfBeg;                // ассоциируем светофор первого составного маршрута
                }
            }
            else
                ret = false;
        }
        indx++;
    }

    if ((srcComplexError = !ret))
        logger.log(QString("Ошибка в описании составного маршрута %1. [Complex]=%2 ").arg(nameLog()).arg(srcComplex));

    return ret;
}

// обработать списки враждебных маршрута
bool Route::checkOpponents (Logger& logger)
{
    bool ret = true;                                        // сбрасывается при хотя бы одной любой ошибке
    if (srcOpponentRoutes.length() > 0)
    {
        QStringList list = srcOpponentRoutes.split(QRegExp("[\\s,\\+]+"));
        foreach (QString s, list)
        {
            bool out = false;
            int no = s.toInt(&out);
            if (out)
            {
                Route * route = st->GetRouteByNo(no);
                if (route != nullptr)
                    listCrossRoutes.append(route);
                else
                    ret = false;
            }
        }
    }
    if ((srcOpponentError = !ret))
        logger.log(QString("Ошибка в описании составного маршрута %1. [Complex]=%2 ").arg(nameLog()).arg(srcComplex));
    return ret;
}


QString Route::nameLog()
{
    return QString("Ст.%1. Маршрут №%2").arg(st->Name()).arg(relNo);
}

// чтение последовательности ТУ, проверка синтаксиса и запись в список
bool Route::parseTuList (QSqlQuery& query, QString field, QString& src, QVector <Tu*> list, Logger& logger)
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
                logger.log(QString("Маршрут %1. Ошибка описания ТУ %2 в маршруте: [%3]='%4'").arg(nameLog()).arg(s).arg(field).arg(src));
                ret = false;
            }
        }
    }
    return ret;
}

// создание вычислителя для обработки формул
bool Route::parseExpression(QSqlQuery& query, QString field, QString& src, BoolExpression *& expr, Logger& logger)
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
            logger.log(QString("Маршрут %1. Ошибка выражения в поле %2 '%3': %4").arg(nameLog()).arg(field).arg(expr->Source()).arg(expr->ErrorText()));
        }
    }
    return true;
}

// получить перечисление идентифицированных РЦ маршрута
QString Route::GetRcEnum()
{
    QString ret;
    foreach (Rc * rc, listRc)
        ret += rc->Name() + " ";
    return ret;
}

// получить перечисление идентифицированных стрелок маршрута
QString Route::GetStrlEnum()
{
    QString ret;
    foreach(LinkedStrl * link, listStrl)
        ret += link->Name() + " ";
    return ret;
}
// получить перечисление ТУ установки
QString Route::GetTuSetEnum()
{
    QString ret;
    foreach(Tu * tu, tuSetList)
        ret += tu->Name() + " ";
    return ret;

}

// получить перечисление ТУ отмены
QString Route::GetTuCancelEnum()
{
    QString ret;
    foreach(Tu * tu, tuCancelList)
        ret += tu->Name() + " ";
    return ret;
}

// Проверка открытого состояния ограждающего светофора
bool Route::IsOpen()
{
    return svtfBeg != nullptr && svtfBeg->IsOpen();
}
