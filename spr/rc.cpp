#include "station.h"
#include "ts.h"
#include "tu.h"
#include "rc.h"
#include "properties.h"

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
Rc * Rc::GetByNo(int no)
{
    return rchash.contains(no) ? rchash[no] : nullptr;
}

// TODO:
// получить справочник по номеру РЦ и номеру потока
Rc * Rc::GetSprByOrgNoAndKrug(int no, int bridgeno)
{
    return GetByNo(no);
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


// обработать ТС, помеченный как РЦ
bool Rc::AddTs (Ts * ts, Logger& logger)
{
    // ищем существующую РЦ или добавляем новую
    int no = ts->IdRc();
    Rc * rc = rchash.contains(no) ? rchash[no] : new Rc(ts, logger);

    // теперь нужно выполнить привязку свойства
    if (ts->IsBusy())
        rc->busy->SetTs(ts);
    else
    {
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
    }

    return ts->IsParsed();
}

// обработать ТУ, помеченный как РЦ
bool Rc::AddTu (Tu * tu, Logger& logger)
{
    // ищем существующую РЦ или добавляем новую
    int no = tu->IdRc();
    Rc * rc = rchash.contains(no) ? rchash[no] : new Rc(tu, logger);

    rc->tulock      ->Parse(tu, logger);
    rc->tuunlock    ->Parse(tu, logger);
    rc->tuir        ->Parse(tu, logger);

    if (!tu->IsParsed())
    {
        logger.log(QString("%1: не идентифицирована ТУ для РЦ").arg(tu->NameEx()));
    }
    return tu->IsParsed();
}
