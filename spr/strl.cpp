#include "station.h"
#include "ts.h"
#include "strl.h"
#include "properties.h"

QHash<QString, class IdentityType *> Strl::propertyIds;     //  множество шаблонов возможных свойств СТРЛ
QHash<QString, class IdentityType *> Strl::methodIds;       //  множество шаблонов возможных методов СТРЛ
QHash <int, Strl *> Strl::strlhash;                         // СТРЛ , индексированные по индексу ТС

Strl::Strl(SprBase * tuts, Logger& logger)
{
    formula_zs = nullptr;
    formula_mu = nullptr;

    SetBaseType(BaseStrl);

    no = tuts->IdStrl();                                    // в общем случае идентификация в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
    nost = tuts->IdSt();                                    // номер станции
    st   = tuts->St();                                      // справочник

    // формируем свойства
    vzrez           = new Property("взрез"                   , propertyIds, logger);
    selectedunlock  = new Property("выбор для разблокировки" , propertyIds, logger);
    selectedvsa     = new Property("выбор для СА"            , propertyIds, logger);
    selectedvsa_p   = new Property("выбор для СА-"           , propertyIds, logger);
    selectedvsa_m   = new Property("выбор для СА+"           , propertyIds, logger);
    locked          = new Property("запирание"               , propertyIds, logger);
    minus           = new Property("контроль минус"          , propertyIds, logger);
    plus            = new Property("контроль плюс"           , propertyIds, logger);
    mu              = new Property("местное управление"      , propertyIds, logger);


    // формируем методы
    selectvsa   = new Method  ("выбор для СА"                , methodIds , logger);
    selectvsa_p = new Method  ("выбор для СА-"               , methodIds , logger);
    selectvsa_m = new Method  ("выбор для СА+"               , methodIds , logger);
    lock        = new Method  ("запирание"                   , methodIds , logger);
    unlock      = new Method  ("отпирание"                   , methodIds , logger);
    setplus     = new Method  ("перевод в минус"             , methodIds , logger);
    setminus    = new Method  ("перевод в плюс"              , methodIds , logger);

    strlhash[no] = this;                                    // добавляем в общую таблицу СТРЛ
    tuts->St()->AddStrl(this, logger);                      // добавляем в таблицу СТРЛ станции

    // если объект конструируем по ТУ, значит не было ТС - ущербное описание объекта
    if (tuts->GetBaseType()==BaseTu)
        logger.log(QString("Описание стрелки %1 не содержит сигналов ТС").arg(NameEx()));

}


Strl::~Strl()
{

}


// проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
bool Strl::AddTemplate(IdentityType * ident)
{
    if (ident->ObjType() == "СТРЛ")
    {
        if (ident->PropType() == "ТС")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}

bool Strl::AddTs (QSqlQuery& query, Ts * ts, Logger& logger)
{
    int no = ts->IdStrl();
    Strl * strl = strlhash.contains(no) ? strlhash[no] : new Strl(ts, logger);

    // выражение для замыкания стрелок
    QString s = query.value("StrlZsName" ).toString();
    if (strl->formula_zs==nullptr)
    {
        strl->formula_zs = new BoolExpression(s);
        if (strl->formula_zs->Valid())
            QObject::connect(strl->formula_zs, SIGNAL(GetVar(QString&,int&)), strl->st, SLOT(GetValue(QString&,int&)));
        else
            logger.log(QString("%1. Ошибка синтаксиса в поле ExtData '%2': %3").arg(strl->NameEx()).arg(strl->formula_zs->Source()).arg(strl->formula_zs->ErrorText()));

    }
    else
    if (s != strl->formula_zs->Source())
    {
        logger.log(QString("%1. Переопределение выражения для замыкания стрелки '%2': %3").arg(strl->NameEx()).arg(strl->formula_zs->Source()).arg(strl->formula_zs->ErrorText()));
    }
/*
    // выражение для контроля местного управления стрелкой
    s = query.value("StrlMuName" ).toString();
    if (strl->strlmuname.length()==0)
        strl->strlmuname = s;
    else
    if (s != strl->strlmuname)
    {

    }
*/
    strl->tsList.append(ts);
    strl->vzrez          ->Parse(ts, logger);
    strl->selectedunlock ->Parse(ts, logger);
    strl->selectedvsa    ->Parse(ts, logger);
    strl->selectedvsa_p  ->Parse(ts, logger);
    strl->selectedvsa_m  ->Parse(ts, logger);
    strl->locked         ->Parse(ts, logger);
    strl->minus          ->Parse(ts, logger);
    strl->plus           ->Parse(ts, logger);
    strl->mu             ->Parse(ts, logger);

    if (!ts->IsParsed())
    {
        logger.log(QString("%1: не идентифицирован контроль СТРЛ").arg(ts->NameEx()));
    }

    return true;
}

bool Strl::AddTu (QSqlQuery& query, Tu * tu, Logger& logger)
{
    // ищем существующую стрелку или добавляем новую
    int no = tu->IdStrl();
    Strl * strl = strlhash.contains(no) ? strlhash[no] : new Strl(tu, logger);

    strl->tuList.append(tu);
    strl->selectvsa   ->Parse(tu, logger);
    strl->selectvsa_p ->Parse(tu, logger);
    strl->selectvsa_m ->Parse(tu, logger);
    strl->lock        ->Parse(tu, logger);
    strl->unlock      ->Parse(tu, logger);
    strl->setplus     ->Parse(tu, logger);
    strl->setminus    ->Parse(tu, logger);
    if (!tu->IsParsed())
    {
        logger.log(QString("%1: не идентифицирована ТУ для СТРЛ").arg(tu->NameEx()));
    }

    return true;
}
