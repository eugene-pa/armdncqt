#include "station.h"
#include "ts.h"
#include "rc.h"
#include "properties.h"

QHash<QString, class IdentityType *> Rc::propertyIds;       // множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Rc::methodIds;         // множество шаблонов возможных методов РЦ
QHash <int, Rc *> Rc::rchash;                               // РЦ , индексированные по индексу ТС

Rc::Rc(Ts * ts, Logger& logger)
{
    no = ts->IdRc();                                        // в общем случае идентификация РЦ в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц

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

    rchash[no] = this;
    ts->St()->AddRc(this, logger);
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

bool Rc::AddTs (Ts * ts, Logger& logger)
{
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

    return true;
}
