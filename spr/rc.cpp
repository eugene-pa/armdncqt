#include "station.h"
#include "krug.h"

QHash<QString, class IdentityType *> Rc::propertyIds;       // множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Rc::methodIds;         // множество шаблонов возможных методов РЦ
QHash <int, Rc *> Rc::rchash;                               // РЦ , индексированные по индексу ТС

Rc::Rc(SprBase * tuts, Logger& logger)
{
    SetBaseType(BaseRc);

    no   = tuts->NoRc();                                    // в общем случае идентификация РЦ в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
    nost = tuts->NoSt();                                    // номер станции
    st   = tuts->St();                                      // справочник
    krug = tuts->Krug();                                    // круг

    actualRoute = nullptr;
    actualtrain = nullptr;
    prvActual = nullptr;                                    // активная связь влево  по стрелкам
    nxtActual = nullptr;                                    // активная связь вправо по стрелкам

    stsBusy         = false;
    stsBusyFalse    = false;
    stsBusyPrv      = false;
    stsZmk          = false;
    stsIr           = false;
    stsMu           = false;
    stsBlock        = false;
    stsBusyPulse    = false;
    stsZmkPulse     = false;
    stsRouteRq      = false;
    stsRouteOk      = false;
    stsPassed       = false;

    // формируем свойства РЦ
    locked      = new Property("блокировка"                 , propertyIds, logger);
    unlocking   = new Property("восприятие разблокировки"   , propertyIds, logger);
    selected_ir = new Property("выбор для разделки"         , propertyIds, logger);
    zmk         = new Property("замыкание"                  , propertyIds, logger);
    ir          = new Property("искусственная разделка"     , propertyIds, logger);
    busy        = new Property("занятость"                  , propertyIds, logger);
    falsebusy   = new Property("ложная занятость"           , propertyIds, logger);
    mu          = new Property("местное управление"         , propertyIds, logger);
    uri         = new Property("разделка"                   , propertyIds, logger);

    // формируем методы РЦ
    tulock      = new Method  ("блокировка"                 , methodIds , logger);
    tuunlock    = new Method  ("разблокировка"              , methodIds , logger);
    tuir        = new Method  ("искусственная разделка"     , methodIds , logger);

    rchash[Id()] = this;                                    // добавляем в общую таблицу РЦ
    tuts->St()->AddRc(this, logger);                        // добавляем в таблицу РЦ станции

    // если объект конструируем по ТУ, значит не было ТС - ущербное описание объекта
    if (tuts->GetBaseType()==BaseTu)
        logger.log(QString("Описание РЦ %1 не содержит сигналов ТС").arg(NameEx()));
}


Rc::~Rc()
{
    delete locked;
    delete unlocking;
    delete selected_ir;
    delete zmk;
    delete busy;
    delete ir;
    delete falsebusy;
    delete mu;
    delete uri;

    delete tulock;
    delete tuunlock;
    delete tuir;
}

// получить справочник по номеру РЦ
Rc * Rc::GetById(int no, KrugInfo * krug)
{
    int id = krug==nullptr ? no : krug->key(no);
    return rchash.contains(id) ? rchash[id] : nullptr;
}


// проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
bool Rc::AddTemplate(IdentityType * ident)
{
    if (ident->ObjType() == "РЦ")
    {
        if (ident->PropType() == "ТС")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}


// Обработать ТС, помеченный как РЦ
// Принято компромиссное решение: ТС хранит бОльшую часть полей из таблицы TS, специфические же для объекта поля читаются самими объектами
// Для этого приходится передавать в функции ссылку на запись в БД: QSqlQuery&
Rc * Rc::AddTs (QSqlQuery& query, Ts * ts, Logger& logger)
{
    // ищем существующую РЦ или добавляем новую
    int id = ts->IdRc();
    Rc * rc = rchash.contains(id) ? rchash[id] : new Rc(ts, logger);

    bool busy = query.value("Occupation").toBool();

    // теперь нужно выполнить привязку свойства
    if (busy)
    {
        rc->allts.append(ts);
        rc->busy->SetTs(ts);
        rc->name = ts->Name();                              // имя РЦ определяем по имени сигнала занятости
        rc->disabled = ts->disabled;
        rc->tpoint = rc->name.indexOf("Т")==0;              // признак того, что это перегонная точка (имя сигнала начинается с "Т")

        // читаем специфические для РЦ данные из таблицы ТС (только для сигналов занятости)
        bool ret = true;
        rc->pathno      = query.value("Path"        ).toInt (&ret);
        rc->pregonno    = query.value("NoPrg"       ).toInt (&ret);
        rc->distance    = query.value("Distance"    ).toFloat(&ret);
        rc->breaked     = query.value("LinkBlind"   ).toBool();
        QString dir     = query.value("Type").toString();
        rc->dir         = dir.indexOf("O") == 0 ? -1 : dir.indexOf("E") == 0 ? 1 : 0;
        rc->rcTopoType  = (Rc::RcTypes)(query.value("TypLM").toInt (&ret)); // вычисляется автоматически, поэтому не имеет особого сиысла

        // Необработанные поля:
        // First_Last
        // Branch
        // SendToNext
        // Alias
        // TimeRemoveNoTr
        // InvPulse
        // Park
    }
    else
    {
        rc->tsList.append(ts);
        rc->locked      ->Parse(ts, logger);
        rc->unlocking   ->Parse(ts, logger);
        rc->selected_ir ->Parse(ts, logger);
        rc->zmk         ->Parse(ts, logger);
        rc->ir          ->Parse(ts, logger);
        rc->falsebusy   ->Parse(ts, logger);
        rc->mu          ->Parse(ts, logger);
        rc->uri         ->Parse(ts, logger);

        if (!ts->IsParsed())
        {
            logger.log(QString("%1: не идентифицирован контроль РЦ").arg(ts->NameEx()));
        }
        else
            rc->allts.append(ts);
    }

    return rc;
}

// обработать ТУ, помеченный как РЦ
Rc * Rc::AddTu (QSqlQuery& query, Tu * tu, Logger& logger)
{
    Q_UNUSED(query)

    // ищем существующую РЦ или добавляем новую
    int id = tu->IdRc();
    Rc * rc = rchash.contains(id) ? rchash[id] : new Rc(tu, logger);
    rc->tuList.append(tu);

    rc->tulock      ->Parse(tu, logger);
    rc->tuunlock    ->Parse(tu, logger);
    rc->tuir        ->Parse(tu, logger);

    if (!tu->IsParsed())
    {
        logger.log(QString("%1: не идентифицирована ТУ для РЦ").arg(tu->NameEx()));
    }
    else
        rc->alltu.append(tu);
    return rc;
}


// чтение таблицы связей РЦ (RC_Relations) и формирование дерева связей
// Чтение выполняется ПОСЛЕ чтения ТУ/ТС но ДО чтения маршрутов
// Функция учитывает доп.поля с информацией о разделяющих РЦ светофорах:
// LR_svtf	- поездной   при движении слева направо
// LR_svtfM	- маневровый при движении слева направо
// RL_svtf	- поездной   при движении справа налево
// RL_svtfM	- маневровый при движении справа налево
//
// Если есть разделюящие светофоры, в их справочники прописываются РЦ перед/за светофором (RcBefore, RcAfter)
// Каждая связь прописывается:
//	- в левую РЦ, записывая в нее светофоры при движении слева направо
//  - в правую РЦ, записывая в нее светофоры при движении справо налево
// Таким образом при вычислении соседей будут вычислены соседние РЦ и их связи, причем связи сразу будут содержать разделяющие светофоры в нужном направлении

bool Rc::ReadRelations(QString& dbpath, Logger& logger)
{
    bool ret = true;
    logger.log(QString("Чтение таблицы [RC_Relations] БД %1").arg(dbpath));
    QString sql("SELECT * FROM [RC_Relations] ORDER BY RC_Left, RC_Right");

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
                    // читаем поля левой и правой РЦ
                    int lft = query.value("RC_Left"    ).toInt(&ret);
                    int rht = query.value("RC_Right"   ).toInt(&ret);

                    // читаем новые поля разделяющих светофоров
                    int nsvtf_LR     = query.value("LR_svtf"  ).toInt(&ret);// поездной   при движении слева направо
                    int nsvtf_LR_M   = query.value("LR_svtfM" ).toInt(&ret);// маневровый при движении слева направо
                    int nsvtf_RL     = query.value("RL_svtf"  ).toInt(&ret);// поездной   при движении справа налево
                    int nsvtf_RL_M   = query.value("RL_svtfM" ).toInt(&ret);// маневровый при движении справа налево

                    QString header = QString("Таблица 'RC_Relations'. Связь РЦ %1/%2; Свтф: %3,%4,%5,%6").arg(lft).arg(rht).arg(nsvtf_LR).arg(nsvtf_LR_M).arg(nsvtf_RL).arg(nsvtf_RL_M);		// заголовок сообщения
                    Svtf * svtf_LR  = Svtf::GetById(nsvtf_LR  );
                    Svtf * svtf_LRM = Svtf::GetById(nsvtf_LR_M);
                    Svtf * svtf_RL  = Svtf::GetById(nsvtf_RL  );
                    Svtf * svtf_RLM = Svtf::GetById(nsvtf_RL_M);

                    if (	(svtf_LR == nullptr &&nsvtf_LR  )
                        ||  (svtf_LRM== nullptr &&nsvtf_LR_M)
                        ||  (svtf_RL == nullptr &&nsvtf_RL  )
                        ||  (svtf_RLM== nullptr &&nsvtf_RL_M)
                        )
                    {
                        logger.log(QString("%1. Хотя бы один из светофоров связи не найден в справочниках. Проверьте светофоры в полях LR[RL]_svtf").arg(header));
                    }

                    Rc * l = Rc::GetById(lft);              // Левая РЦ в связи
                    Rc * r = Rc::GetById(rht);              // Правая РЦ в связи
                    if (   l == nullptr || r == nullptr || l == r)
                    {
                        logger.log(QString("%1. Некорректная пара РЦ").arg(header));
                        continue;
                    }

                    // Если есть разделюящие светофоры, прописываю в их справочники РЦ перед/за светофором (RcBefore, RcAfter)
                    if (svtf_LR)                            // поездной   при движении слева направо
                    {
                        svtf_LR->SetRcBefore(l);
                        svtf_LR->SetRcAfter (r);
                    }
                    if (svtf_LRM)                           // маневровый при движении слева направо
                    {
                        svtf_LRM->SetRcBefore(l);
                        svtf_LRM->SetRcAfter (r);
                    }
                    if (svtf_RL)                            // поездной   при движении справа налево
                    {
                        svtf_RL->SetRcBefore(r);
                        svtf_RL->SetRcAfter (l);
                    }
                    if (svtf_RLM)                           // маневровый при движении справа налево
                    {
                        svtf_RLM->SetRcBefore(r);
                        svtf_RLM->SetRcAfter (l);
                    }

                    NxtPrv * lnk = new NxtPrv(l, r, svtf_LR, svtf_LRM, svtf_RL, svtf_RLM);
                    const int maxstrl = 4;
                    for (int i=1; i<=maxstrl; i++)
                    {
                        int no = 0;
                        QString id  = "Strl" + QString::number(i);
                        QString sts = "Sts"  + QString::number(i);
                        no = query.value(id).toInt(&ret);
                        bool sign = query.value(sts).toBool();
                        if (no)
                            lnk->strl.append(new LinkedStrl(sign ? -no : no));
                    }

                    l->nxt.append(lnk);
                    r->prv.append(lnk);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Ts::ReadBd");
        ret = false;
    }

    return ret;
}

// обработка объектов по станции
void Rc::AcceptTS (Station *st)
{
    for(auto map : st->Allrc())
    {
        map.second->Accept();
    }
}

// очистка списка примитивов РЦ
void Rc::ClearShapes()
{
    foreach(Rc * rc, Rc::AllRc().values())
    {
        rc->shapes.clear();
    }

}

// обработка ТС
void Rc::Accept()
{
    stsBusyPrv  = stsBusy;
    stsBusy     = SafeValue(busy);
    stsZmk      = SafeValue(zmk);
    stsIr       = SafeValue(ir) || SafeValue(uri) || SafeValue(selected_ir) || SafeValue(unlocking);
    stsMu       = SafeValue(mu);
    stsBlock    = SafeValue(locked);
    stsBusyFalse= SafeValue(falsebusy);

    stsRouteRq  = ActualRoute() == nullptr ? false : ActualRoute()->StsRqSet(); // в устанавливаемом маршруте
    stsRouteOk  = ActualRoute() == nullptr ? false : ActualRoute()->StsOn();    // в неразделанном маршруте

    // состояние stsPassed вычисляется при первичной обработке и передается в потоке дальше
    if (stsBusy && ActualRoute())
        stsPassed   = true;

//  stsBusyPulse= SafeValue();                          // РЕТАЙМ. мигает занятость     - ???
//  stsZmkPulse = SafeValue();                          // РЕТАЙМ. мигает замыкание     - ???

    // состояние в терминах УП
    int unistatus = (int)GetUniStatus();
    if (uniSts != unistatus)
    {
        uniSts = unistatus;                             // унифицированное состояние
        uniStsChanged = true;                           // пометка изменения состояния в терминах унифицированного состояния
    }
}

// получить статус UNI
SprBase::UniStatusRc Rc::GetUniStatus()
{
    // TODO!!!
    return SprBase::StsFreeUnlocked;
}

QString Rc::About()
{
    QString s = "Ст." + StationName();
    s += ". РЦ " + Name();

    s += busy       ->About();
    s += zmk        ->About();
    s += ir         ->About();
    s += selected_ir->About();
    s += uri        ->About();
    s += locked     ->About();
    s += unlocking  ->About();
    s += falsebusy  ->About();
    s += mu         ->About();

    s += tuir       ->About();
    s += tulock     ->About();
    s += tuunlock   ->About();

    return s;
}
