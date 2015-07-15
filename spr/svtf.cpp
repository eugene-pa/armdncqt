#include "station.h"
#include "ts.h"
#include "svtf.h"
#include "properties.h"

QHash<QString, class IdentityType *> Svtf::propertyIds;     //  множество шаблонов возможных свойств СВТФ
QHash<QString, class IdentityType *> Svtf::methodIds;       //  множество шаблонов возможных методов СВТФ
QHash <int, Svtf *> Svtf::svtfhash;                         // СВТФ , индексированные по индексу ТС

Svtf::Svtf(SprBase * tuts, Logger& logger)
{
    SetBaseType(BaseSvtf);

    no = tuts->IdSvtf();                                    // в общем случае идентификация в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
    nost = tuts->IdSt();                                    // номер станции
    st   = tuts->St();                                      // справочник

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

    svtfhash[no] = this;                                    // добавляем в общую таблицу СВТФ
    tuts->St()->AddSvtf(this, logger);                      // добавляем в таблицу СВТФ станции

    // если объект конструируем по ТУ, значит не было ТС - ущербное описание объекта
    if (tuts->GetBaseType()==BaseTu)
        logger.log(QString("Описание светофора %1 не содержит сигналов ТС").arg(NameEx()));
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

bool Svtf::AddTs (QSqlQuery& query, Ts * ts, Logger& logger)
{
    int no = ts->IdSvtf();
    Svtf * svtf = svtfhash.contains(no) ? svtfhash[no] : new Svtf(ts, logger);

    // добираем нужные поля
    svtf->svtfdiag      = query.value("SvtfDiag"  ).toString();     // тип диагностического контроля

    bool   svtfmain = query.value("SvtfMain").toBool();
    // теперь нужно выполнить привязку свойства
    if (svtfmain)
    {
        svtf->opened->SetTs(ts);
        svtf->svtferror     = query.value("SvtfError" ).toString(); // выражение ошибки светофора
        svtf->svtftypename  = query.value("SvtfClass" ).toString().toUpper();   // имя типа
        svtf->svtftype= svtf->svtftypename == "ВХ"  ? SVTF_X   :// входной
                        svtf->svtftypename == "ВЫХ" ? SVTF_IN  :// выходной
                        svtf->svtftypename == "МРШ" ? SVTF_OUT :// маршрутный
                        svtf->svtftypename == "ПРХ" ? SVTF_PRH :// проходной
                        svtf->svtftypename == "МНВ" ? SVTF_MNV :// маневровый
                        svtf->svtftypename == "ПРС" ? SVTF_PRLS:// пригласительный
                                                      SVTF_X;
    }
    else
    {
        svtf->tsList.append(ts);
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
            if (svtf->svtfdiag.length()==0)
            {
                logger.log(QString("%1: не идентифицирован контроль СВТФ").arg(ts->NameEx()));
                return false;
            }
        }
    }

    return true;
}

bool Svtf::AddTu (QSqlQuery& query, Tu * tu, Logger& logger)
{
    int no = tu->IdSvtf();
    Svtf * svtf = svtfhash.contains(no) ? svtfhash[no] : new Svtf(tu, logger);
    svtf->tuList.append(tu);

    // выполняем привязку метода
    svtf->open     ->Parse(tu, logger);
    svtf->close    ->Parse(tu, logger);
    svtf->lock     ->Parse(tu, logger);
    svtf->unlock   ->Parse(tu, logger);
    svtf->adon     ->Parse(tu, logger);
    svtf->adoff    ->Parse(tu, logger);
    svtf->mm       ->Parse(tu, logger);
    svtf->cancel   ->Parse(tu, logger);
    return true;
}
