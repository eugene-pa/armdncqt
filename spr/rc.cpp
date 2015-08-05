#include "station.h"

QHash<QString, class IdentityType *> Rc::propertyIds;       // множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Rc::methodIds;         // множество шаблонов возможных методов РЦ
QHash <int, Rc *> Rc::rchash;                               // РЦ , индексированные по индексу ТС

Rc::Rc(SprBase * tuts, Logger& logger)
{
    SetBaseType(BaseRc);

    no   = tuts->IdRc();                                    // в общем случае идентификация РЦ в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
    nost = tuts->IdSt();                                    // номер станции
    st   = tuts->St();                                      // справочник

    actualRoute = nullptr;
    actualtrain = nullptr;


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

    rchash[no] = this;                                      // добавляем в общую таблицу РЦ
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
Rc * Rc::GetById(int no)
{
    return rchash.contains(no) ? rchash[no] : nullptr;
}

// TODO:
// получить справочник по номеру РЦ и номеру потока
Rc * Rc::GetSprByOrgNoAndKrug(int no, int bridgeno)
{
    return GetById(no);
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
    int no = ts->IdRc();
    bool ret;
    Rc * rc = rchash.contains(no) ? rchash[no] : new Rc(ts, logger);

    bool busy = query.value("Occupation").toBool();

    // теперь нужно выполнить привязку свойства
    if (busy)
    {
        rc->allts.append(ts);
        rc->busy->SetTs(ts);
        rc->name = ts->Name();                              // имя РЦ определяем по имени сигнала занятости
        rc->tpoint = rc->name.indexOf("Т")==0;              // признак того, что это перегонная точка (имя сигнала начинается с "Т")
        if (rc->tpoint)
            int a=99;

        // читаем специфические для РЦ данные из таблицы ТС (только для сигналов занятости)
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
    // ищем существующую РЦ или добавляем новую
    int no = tu->IdRc();
    Rc * rc = rchash.contains(no) ? rchash[no] : new Rc(tu, logger);
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
