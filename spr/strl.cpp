    #include "station.h"
#include "ts.h"
#include "strl.h"
#include "properties.h"

QHash<QString, class IdentityType *> Strl::propertyIds;     //  множество шаблонов возможных свойств СТРЛ
QHash<QString, class IdentityType *> Strl::methodIds;       //  множество шаблонов возможных методов СТРЛ
QHash <int, Strl *> Strl::strlhash;                         // СТРЛ , индексированные по индексу ТС

// -----------------------------------------------------------
// конструктор стрелки по номеру со знаком
LinkedStrl::LinkedStrl(int no)
{
    this->no = no;
    strl = Strl::GetById(abs(no));
    name = strl != nullptr ? strl->basename + (no > 0 ? "+" : "-") : "?";
}

// конструктор по имени контроля +/- и станции
LinkedStrl::LinkedStrl(class Station* st, QString& name)
{
    strl = st->GetStrlByName(name, no);
    this->name = strl != nullptr ? strl->basename + (no > 0 ? "+" : "-") : "?";
}

// -----------------------------------------------------------

Strl::Strl(SprBase * tuts, Logger& logger)
{
    formula_zs = nullptr;
    formula_mu = nullptr;

    SetBaseType(BaseStrl);

    no   = tuts->NoStrl();                                  // номер стрелки из БД
    nost = tuts->NoSt();                                    // номер станции из БД
    st   = tuts->St();                                      // справочник
    krug = tuts->Krug();                                    // круг

    sprRc   = nullptr;                                      // РЦ, в которую входит стрелка
    sprRc2  = nullptr;                                      // вторая РЦ, в которую входит спаренная стрелка
    stsPls  = false;                                        // +
    stsMns = false;                                         // -
    stsPlsPrv = false;                                      // + в пред.шаге
    stsMnsPrv = false;                                      // - в пред.шаге
    stsPlsRq = false;                                       // переводим в +
    stsMnsRq = false;                                       // переводим в -
    stsInformed = false;                                    // уведомляли
    stsMu = false;                                          // МУ
    stsZs = false;                                          // ЗС
    stsZpr = false;                                         // ЗПР
    stsOtuPlus = false;                                     // ВСА в +
    stsOtuMinus = false;                                    // ВСА в -
    stsOtu = false;                                         // ВСА
    stsVzres = false;                                       // взрез
    stsSelRsbl = false;                                     // выбор для разблокировки
    stsBusyRc = false;                                      // занятость РЦ под стрелкой
    stsBusyAnyRc = false;                                   // занятость любой из двух РЦ под спаренными стрелками
    stsZmkRc = false;                                       // замыкание РЦ под стрелкой
    stsZmkAnyRc = false;                                    // замыкание любой из двух РЦ под спаренными стрелками
    stsIrRc = false;                                        // ИР РЦ под стрелкой
    stsIrAnyRc = false;                                     // ИР любой из двух РЦ под спаренными стрелками

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

    strlhash[Id()] = this;                                  // добавляем в общую таблицу СТРЛ
    tuts->St()->AddStrl(this, logger);                      // добавляем в таблицу СТРЛ станции

    // если объект конструируем по ТУ, значит не было ТС - ущербное описание объекта
    if (tuts->GetBaseType()==BaseTu)
        logger.log(QString("Описание стрелки %1 не содержит сигналов ТС").arg(NameEx()));

}


Strl::~Strl()
{

}

void Strl::SetRc(class Rc* rc)
{
    if (sprRc != rc && sprRc2!=rc)
    {
        if (sprRc == nullptr)
            sprRc = rc;
        else
        if (sprRc2 == nullptr)
            sprRc2 = rc;
        else
        {
            qDebug() << "Too maany RC for STRL" << this;
        }
    }
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
    int id = ts->IdStrl();
    Strl * strl = strlhash.contains(id) ? strlhash[id] : new Strl(ts, logger);

    bool pls = strl->plus           ->Parse(ts, logger);
    strl->tsList.append(ts);
    strl->vzrez          ->Parse(ts, logger);
    strl->selectedunlock ->Parse(ts, logger);
    strl->selectedvsa    ->Parse(ts, logger);
    strl->selectedvsa_p  ->Parse(ts, logger);
    strl->selectedvsa_m  ->Parse(ts, logger);
    strl->locked         ->Parse(ts, logger);
    strl->minus          ->Parse(ts, logger);
    strl->mu             ->Parse(ts, logger);
    strl->disabled      |= ts->disabled;

    if (!ts->IsParsed())
    {
        logger.log(QString("%1: не идентифицирован контроль СТРЛ").arg(ts->NameEx()));
    }

    if (pls)
    {

        strl->basename = strl->name = ts->name;            // "конечное" имя - плюс
        strl->name.replace("+", "±");
        strl->basename.replace("+", "");
    }
    else
    if (strl->name.length()==0)
    {
        strl->name = ts->name;                              // промежуточное имя для лога
    }


    // принимаем описание формул для замыкания стрелок и местного управления, создаем вычислители и сразу проверяем валидность
    // выражение для замыкания стрелок
    // анализируем вычислитель formula_zs, если он "пустой" - конструируем,
    QString s = query.value("StrlZsName" ).toString();
    if (s.length())
    {
        BoolExpression*& f = strl->formula_zs;              // ССЫЛКА НА УКАЗАТЕЛЬ
        if (f==nullptr)
        {
            f = new BoolExpression(s);
            if (f->Valid())
                QObject::connect(f, SIGNAL(GetVar(QString&,int&)), strl->st, SLOT(GetValue(QString&,int&)));
            else
                logger.log(QString("%1. Ошибка синтаксиса в поле StrlZsName '%2': %3").arg(strl->NameEx()).arg(f->Source()).arg(f->ErrorText()));

        }
        else
        if (s != f->Source())
        {
            logger.log(QString("%1. Переопределение выражения для замыкания стрелки '%2' <> %3").arg(strl->NameEx()).arg(f->Source()).arg(s));
        }
    }

    // выражение для замыкания стрелок
    // анализируем вычислитель formula_zs, если он "пустой" - конструируем,
    s = query.value("StrlMuName" ).toString();
    BoolExpression*& f = strl->formula_mu;
    if (s.length())
    {
        if (f==nullptr)
        {
            f = new BoolExpression(s);
            if (f->Valid())
                QObject::connect(f, SIGNAL(GetVar(QString&,int&)), strl->st, SLOT(GetValue(QString&,int&)));
            else
                logger.log(QString("%1. Ошибка синтаксиса в поле StrlMuName '%2': %3").arg(strl->NameEx()).arg(f->Source()).arg(f->ErrorText()));

        }
        else
        if (s != f->Source())
        {
            logger.log(QString("%1. Переопределение выражения для МУ стрелки: '%2' <> %3").arg(strl->NameEx()).arg(f->Source()).arg(s));
        }
    }

    return true;
}

bool Strl::AddTu (QSqlQuery& query, Tu * tu, Logger& logger)
{
    Q_UNUSED(query)

    // ищем существующую стрелку или добавляем новую
    int id = tu->IdStrl();
    Strl * strl = strlhash.contains(id) ? strlhash[id] : new Strl(tu, logger);

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

// получить справочник по номеру светофора
Strl * Strl::GetById(int no)
{
    return strlhash.contains(no) ? strlhash[no] : nullptr;
}

// обработка объектов по станции
void Strl::AcceptTS (Station *st)
{
    foreach(Strl * strl, st->Allstrl().values())
    {
        strl->Accept();
    }
}

void Strl::Accept ()
{
    stsPlsPrv = stsPls;                                     // + в пред.шаге
    stsMnsPrv = stsMns;                                     // - в пред.шаге
    stsPls    = SafeValue(plus);                            // +
    stsMns    = SafeValue(minus);                           // -
    stsZs     = formula_zs == nullptr ? false : formula_zs->ValueBool(); // ЗС
    stsZpr    = SafeValue(locked);                          // ЗПР
    stsOtuPlus= SafeValue(selectedvsa_p);                   // ВСА в +
    stsOtuMinus=SafeValue(selectedvsa_m);                   // ВСА в -
    stsOtu    = SafeValue(selectedvsa) | stsOtuPlus | stsOtuMinus;// ВСА
    stsVzres  = SafeValue(vzrez);                           // взрез
    stsSelRsbl= SafeValue(selectedunlock);                  // выбор для разблокировки

    // Здесь надо уметь обрабатывать и отдельно записанные выражения МУ
    stsMu     = SafeValue(mu) || (formula_mu != nullptr && formula_mu->ValueBool());// МУ

    stsBusyRc = (sprRc != nullptr) && sprRc->StsBusy();       // занятость РЦ под стрелкой
    stsBusyAnyRc = stsBusyRc || (sprRc2 != nullptr && sprRc2->StsBusy()); // занятость любой из двух РЦ под спаренными стрелками
    stsZmkRc = sprRc != nullptr && sprRc->StsZmk();         // замыкание РЦ под стрелкой
    stsZmkAnyRc = stsZmkRc || (sprRc2 != nullptr && sprRc2->StsZmk()); // замыкание любой из двух РЦ под спаренными стрелками
    stsIrRc = sprRc != nullptr && sprRc->StsIr();           // ИР РЦ под стрелкой
    stsIrAnyRc= stsIrRc || (sprRc2 != nullptr && sprRc2->StsIr()); // ИР любой из двух РЦ под спаренными стрелками

    // состояние в терминах УП
    int unistatus = (int)GetUniStatus();
    if (uniSts != unistatus)
    {
        uniSts = unistatus;                                 // унифицированное состояние
        uniStsChanged = true;                               // пометка изменения состояния в терминах унифицированного состояния
    }
}

// получить статус UNI
SprBase::UniStatusRc Strl::GetUniStatus()
{
    // TODO!!!
    return SprBase::StsFreeUnlocked;
}

QString Strl::About()
{
    QString s = "Ст." + StationName();
    s += ". Стрелка " + Name();

    s += plus           ->About();
    s += minus          ->About();
    s += vzrez          ->About();
    s += selectedunlock ->About();
    s += selectedvsa    ->About();
    s += selectedvsa_p  ->About();
    s += selectedvsa_m  ->About();
    s += locked         ->About();
    s += mu             ->About();

    s += setplus        ->About();
    s += setminus       ->About();
    s += lock           ->About();
    s += unlock         ->About();
    s += selectvsa_p    ->About();
    s += selectvsa_m    ->About();

    return s;
}
