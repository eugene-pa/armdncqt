#include "station.h"
#include "ts.h"
#include "strl.h"
#include "properties.h"

QHash<QString, class IdentityType *> Strl::propertyIds;     //  множество шаблонов возможных свойств СТРЛ
QHash<QString, class IdentityType *> Strl::methodIds;       //  множество шаблонов возможных методов СТРЛ
QHash <int, Strl *> Strl::strlhash;                         // СТРЛ , индексированные по индексу ТС

Strl::Strl(SprBase * tuts, Logger& logger)
{
    SetBaseType(BaseStrl);

    no = tuts->IdSvtf();                                    // в общем случае идентификация в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
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
    setplus     = new Method  ("перевод в минус "            , methodIds , logger);
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
        if (ident->PropType() == "TC")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}

bool Strl::AddTs (Ts * ts, Logger& logger)
{
    return true;
}

bool Strl::AddTu (Tu * tu, Logger& logger)
{
    return true;
}
