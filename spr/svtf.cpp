#include "station.h"
#include "ts.h"
#include "rc.h"
#include "svtf.h"
#include "properties.h"

QHash<QString, class IdentityType *> Svtf::propertyIds;     //  множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Svtf::methodIds;       //  множество шаблонов возможных методов РЦ
QHash <int, Svtf *> Svtf::svtfhash;                         // СВТФ , индексированные по индексу ТС

Svtf::Svtf(Ts * ts, Logger& logger)
{
    no = ts->IdRc();                                        // в общем случае идентификация РЦ в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц

    // формируем свойства
    opened      = new Property("открыт"                     , propertyIds, logger);
    manevr      = new Property("маневровое показание"       , propertyIds, logger);
    calling      = new Property("пригласительный"           , propertyIds, logger);
    pzdM        = new Property("акивен маневровый маршрут"  , propertyIds, logger);
    mnvM        = new Property("активен поездной маршрут"   , propertyIds, logger);
    locked      = new Property("блокировка"                 , propertyIds, logger);
    ad          = new Property("автодействие"               , propertyIds, logger);
    seltounlock = new Property("выбор для разблокировки"    , propertyIds, logger);
    yelllow     = new Property("желтый разрешающий"         , propertyIds, logger);
    ko          = new Property("контроль красного огня"     , propertyIds, logger);
    blinking    = new Property("мигание"                    , propertyIds, logger);
    canceling   = new Property("отмена маршрута"            , propertyIds, logger);

    // формируем методы
    open    = new Method  ("открыть"                        , methodIds , logger);
    close   = new Method  ("закрыть"                        , methodIds , logger);
    lock    = new Method  ("блокировка"                     , methodIds , logger);
    unlock  = new Method  ("разблокировка"                  , methodIds , logger);
    adon    = new Method  ("включить автодействие"          , methodIds , logger);
    adoff   = new Method  ("отключить автодействие"         , methodIds , logger);
    mm      = new Method  ("маршрутная кнопка"              , methodIds , logger);
    cancel  = new Method  ("отмена маршрута"                , methodIds , logger);

}

Svtf::~Svtf()
{

}

// проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
bool Svtf::AddTemplate(IdentityType * ident)
{
    if (ident->ObjType() == "СВТФ")
    {
        if (ident->PropType() == "ТС")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}

bool Svtf::AddTs (Ts * ts, Logger& logger)
{
    int no = ts->IdRc();
    Svtf * svtf = svtfhash.contains(no) ? svtfhash[no] : new Svtf(ts, logger);

    // теперь нужно выполнить привязку свойства
    if (ts->IsSvtfmain())
        svtf->opened->SetTs(ts);
    else
    {
        svtf->manevr      ->Parse(ts, logger);
        svtf->calling     ->Parse(ts, logger);
        svtf->pzdM        ->Parse(ts, logger);
        svtf->mnvM        ->Parse(ts, logger);
        svtf->locked      ->Parse(ts, logger);
        svtf->ad          ->Parse(ts, logger);
        svtf->seltounlock ->Parse(ts, logger);
        svtf->yelllow     ->Parse(ts, logger);
        svtf->ko          ->Parse(ts, logger);
        svtf->blinking    ->Parse(ts, logger);
        svtf->canceling   ->Parse(ts, logger);

        if (!ts->IsParsed())
        {
            if (ts->SvtfDiag().length()==0)
                logger.log(QString("%1: не идентифицирован контроль СВТФ").arg(ts->NameEx()));
        }
    }

    return true;
}
