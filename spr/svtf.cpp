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

    formula_er = nullptr;
    no = tuts->NoSvtf();                                    // в общем случае идентификация в хэш-таблицах должна проиизводиться по ключу: (НомерКруга<<16)|НомерРц
    nost = tuts->NoSt();                                    // номер станции
    st   = tuts->St();                                      // справочник
    krug = tuts->Krug();                                    // круг

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

    svtfhash[Id()] = this;                                  // добавляем в общую таблицу СВТФ
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
    int id = ts->IdSvtf();
    Svtf * svtf = svtfhash.contains(id) ? svtfhash[id] : new Svtf(ts, logger);

    // добираем нужные поля
    svtf->svtfdiag      = query.value("SvtfDiag"  ).toString();     // тип диагностического контроля

    bool   svtfmain = query.value("SvtfMain").toBool();

    // теперь нужно выполнить привязку свойства
    QString tname = query.value("SvtfClass" ).toString().toUpper();   // имя типа
    if (svtfmain && tname!="ПРС")
    {
        svtf->svtftypename = tname;
        svtf->svtftype= tname == "ВХ"  ? SVTF_X   :         // входной
                        tname == "ВЫХ" ? SVTF_IN  :         // выходной
                        tname == "МРШ" ? SVTF_OUT :         // маршрутный
                        tname == "ПРХ" ? SVTF_PRH :         // проходной
                        tname == "МНВ" ? SVTF_MNV :         // маневровый
                        tname == "ПРС" ? SVTF_PRLS:         // пригласительный
                                         SVTF_X;
        if (svtf->svtftype != SVTF_PRLS)
        {
            svtf->name = ts->Name();
            svtf->opened->SetTs(ts);
            svtf->svtferror     = query.value("SvtfError" ).toString(); // выражение ошибки светофора

            // обрабатываем выражеение аварии светофора
            QString s = query.value("SvtfError" ).toString();
            if (s.length())
            {
                svtf->formula_er = new BoolExpression(s);
                if (svtf->formula_er->Valid())
                    QObject::connect(svtf->formula_er, SIGNAL(GetVar(QString&,int&)), svtf->st, SLOT(GetValue(QString&,int&)));
                else
                    logger.log(QString("%1. Ошибка синтаксиса в поле StrlZsName '%2': %3").arg(svtf->NameEx()).arg(svtf->formula_er->Source()).arg(svtf->formula_er->ErrorText()));
            }
        }
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
    Q_UNUSED(query)

    int id = tu->IdSvtf();
    Svtf * svtf = svtfhash.contains(id) ? svtfhash[id] : new Svtf(tu, logger);
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

// получить справочник по номеру светофора
Svtf * Svtf::GetById(int no)
{
    return svtfhash.contains(no) ? svtfhash[no] : nullptr;
}


// обработка объектов по станции
void Svtf::AcceptTS (Station *st)
{
    foreach(Svtf * svtf, st->Allsvtf().values())
    {
        svtf->Accept();
    }
}

void Svtf::Accept()
{
    stsOpen = IsOpen();                                     // открыт

    // состояние в терминах УП
    int unistatus = (int)GetUniStatus();
    if (uniSts != unistatus)
    {
        uniSts = unistatus;                                 // унифицированное состояние
        uniStsChanged = true;                               // пометка изменения состояния в терминах унифицированного состояния
    }
}

// получить статус UNI
SprBase::UniStatusRc Svtf::GetUniStatus()
{
    // TODO!!!
    return SprBase::StsFreeUnlocked;
}
