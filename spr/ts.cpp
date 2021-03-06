#include "station.h"
#include "krug.h"

QString Ts::buf;


// конструктор принимает на входе запись из таблицы TS
Ts::Ts(QSqlQuery& query, KrugInfo * krug, Logger& logger)
{
    this->krug = krug;

    SetBaseType(BaseTs);
    bool ret = true;
    try
    {
        parsed = false;
        iname   = query.value("Cod"     ).toInt(&ret);      // код имени ( в C# было "TS.Cod")
        name    = query.value("NameTs"  ).toString();       // имя ТС
        nost    = query.value("NoSt"    ).toInt(&ret);      // номер станции
        modul   = query.value("Module"  ).toInt(&ret);
        _i      = query.value("I"       ).toInt(&ret);
        _j      = query.value("J"       ).toInt(&ret);
        stativ  = query.value("Stativ"  ).toInt(&ret);
        //place   = query.value("Place"   ).toInt(&ret);    // не используется
        _kolodka= query.value("Kolodka" ).toInt(&ret);
        kontact = query.value("Klem"    ).toString();
        norc    = query.value("NoRc"    ).toInt(&ret);
        nostrl  = query.value("NoStrl"  ).toInt(&ret);
        nosvtf  = query.value("NoSvtf"  ).toInt(&ret);
        disabled= query.value("Lock"    ).toBool();
        inverse = query.value("Inverse" ).toBool();

        pulse   = query.value("Pulse"   ).toBool();
        question= query.value("Question").toString();
        formula = query.value("ExtData" ).toString();

        // Необработанные поля
        // Path
        // NoPrg
        // Distance
        // First_Last
        // Branch
        // LinkBlind
        // Type
        // SendToNext
        // Alias
        // TimeRemoveNoTr
        // TypSvtf              Признак обобщенного  ТСдля  контроля состояния светофоров (как правило выходных)
        // TypLM
        // TypStrl              Тип стрелки (додумать)
        // InvPulse
        // Park


        next = nullptr;
        expression = nullptr;

        st = Station::GetById(nost);
        if (st != nullptr)
        {

            Ts::getIndex(logger);
            if (formula.length())
            {
                // создаем класс для вычисления выражения и связываем его со слотом GetValue класса Station
                expression = new BoolExpression(formula);
                if (expression->Valid())
                    QObject::connect(expression, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
                else
                    logger.log(QString("%1. Ошибка синтаксиса в поле ExtData '%2': %3").arg(NameEx()).arg(expression->Source()).arg(expression->ErrorText()));

                //expression->GetValue();
            }

            if (validIJ)
            {
                // добавляем ТС в справочники Ts, TsIndexed, TsByIndxTsName, TsSorted
                st->AddTs(this, logger);

                // РЦ
                if (norc > 0)
                {
                    Rc::AddTs(query, this, logger);
                }
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoRc: %2").arg(NameEx()).arg(norc));

                // Светофор
                if (nosvtf > 0)
                    Svtf::AddTs(query, this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoSvtf: %2").arg(NameEx()).arg(nosvtf));

                // Стрелка
                if (nostrl > 0)
                    Strl::AddTs(query, this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoStrl: %2").arg(NameEx()).arg(nostrl));

                if (inverse)
                    st->MarkInverse(index);
            }
        }
        else
        {
            logger.log(QString("Некорректный номер станции. Игнорируем сигнал %1").arg(NameEx()));
        }

        kolodka = QString("K%1").arg(_kolodka);
        place = QString("%1:%2").arg(kolodka).arg(kontact);
    }
    catch(...)
    {
        ret = false;
        logger.log("Исключение в конструкторе Ts");
    }
}

Ts::~Ts()
{

}

// чтение БД
bool Ts::ReadBd (QString& dbpath, KrugInfo * krug, Logger& logger)
{
    bool ret = true;
    logger.log(QString("Чтение таблицы [ТS] БД %1").arg(dbpath));
    QString sql("SELECT * FROM [TS] INNER JOIN TS_Name ON TS.Cod = TS_Name.Cod ORDER BY NOST,[Module],[I],[J]");

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
                    new Ts(query, krug, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Ts::ReadBd");
        ret = false;
    }

    logger.log("Сортировка списков ТС");
    Station::SortTs();

    return ret;
}


// сформировать индекс сигнала по координатам
int Ts::getIndex(Logger& logger)
{
    // в конфигурациях МПЦ можем иметь свои модули ТС для контроля ОТУ
    // при этом сигналы Ebilock описываются традиционно координатой  J (0...n-1)
    // поэтому смещаем ккординату на размер "наших" ТС
    if (st->IsMpcEbilock() && st->MtsCount() > 0)
        _j += st->MtsCount() * 32;

    if (st->IsMpcEbilock())
    {
        modul = 1;                                          //
        _i = 1;
    }

    // проверяем допустимость M/I/J c учетом исключений:
    // - ОТПР.Н - исключение (вирт.сигнал для родного ГИД, устарел)
    // - ОТПР.Ч - исключение (вирт.сигнал для родного ГИД, устарел)
    int mmax = 0, imax = 0, jmax = 0, tsPerModule = 0;
    st->GetTsParams(mmax, imax, jmax, tsPerModule);
    validIJ = modul>0 && modul<=mmax && _i>0 && _i<=imax && _j>0 && _j<=jmax;
    if (    !validIJ
        &&  name.indexOf("ОТПР.") != 0)
        logger.log(QString("Ошибка описания m/i/j сигнала %1: %2/%3/%4").arg(NameEx()).arg(modul).arg(_i).arg(_j));
    if (validIJ && st->Kp2000() && _kolodka > 0)
        st->MarkTs(modul);

    return index = (modul - 1) * tsPerModule + (_i - 1) * jmax + _j - 1;
}

int Ts::CompareByNames(const void* p1,const void* p2)
{
    return ((Ts*)p1)->Name() < ((Ts*)p2)->Name();
}

QString Ts::GetTooltip()
{
    QString ret = QString("#%1  %2  %3:%4  %5  %6").arg(index+1).arg(Name()).arg(this->kolodka).arg(kontact).arg(inverse ? "Инверсия" : "").arg(pulse ? "Мигание":"");
    Ts * ts = this;
    while (ts->next != nullptr)
    {
        ts = ts->next;
        ret += "\r\n" + ts->GetTooltip();
    }
    return ret;
}

// значения ТС в итоге получаются обращением к битовым массивам в классе Station
bool Ts::Sts      () { return st->TsSts      (index); }     // состояние (0/1), если мигает - 0
bool Ts::StsPulse () { return st->TsPulse    (index); }     // состояние мигания
bool Ts::StsRaw   () { return st->TsRaw      (index); }     // состояние ненормализованное
bool Ts::Sts_     () { return st->TsStsMoment(index); }     // состояние мгновенное
bool Ts::Value    ()                                        // 0/1 c учетом типа: для мигающих, если мигает, для позиционных 0/1
{
    return pulse ? st->TsPulse(index) : st->TsSts(index);
}
