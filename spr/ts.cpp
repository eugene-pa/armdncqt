#include <QVariant>
#include "sprbase.h"
#include "station.h"
#include "ts.h"
#include "tu.h"
#include "../common/boolexpression.h"
#include "rc.h"
#include "strl.h"
#include "svtf.h"

QString Ts::buf;

Ts::Ts(QSqlQuery& query, Logger& logger)
{
    SetBaseType(BaseTs);
    bool ret;
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
        place   = query.value("Place"   ).toInt(&ret);
        _kolodka= query.value("Kolodka" ).toInt(&ret);
        kontact = query.value("Klem"    ).toString();
        norc    = query.value("NoRc"    ).toInt(&ret);
        nostrl  = query.value("NoStrl"  ).toInt(&ret);
        nosvtf  = query.value("NoSvtf"  ).toInt(&ret);
        locked  = query.value("Lock"    ).toBool();
        inverse = query.value("Inverse" ).toBool();
        busy    = query.value("Occupation").toBool();
        pulse   = query.value("Pulse"   ).toBool();
        svtfmain= query.value("SvtfMain").toBool();
        question= query.value("Question").toString();
        formula = query.value("ExtData" ).toString();

        svtfdiag  = query.value("SvtfDiag" ).toString();
        svtftype  = query.value("SvtfClass" ).toString();
        svtferror = query.value("SvtfError" ).toString();
        strlzsname= query.value("StrlZsName" ).toString();
        strlmuname= query.value("StrlMuName" ).toString();

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

        st = Station::GetByNo(nost);
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
                    Rc::AddTs(this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoRc: %2").arg(NameEx()).arg(norc));

                // Светофор
                if (nosvtf > 0)
                    Svtf::AddTs(this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoSvtf: %2").arg(NameEx()).arg(nosvtf));

                // Стрелка
                if (nostrl > 0)
                    Strl::AddTs(this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoStrl: %2").arg(NameEx()).arg(nostrl));

                if (inverse)
                    ;
            }
        }
        else
        {
            logger.log(QString("Некорректный номер станции. Игнорируем сигнал %1").arg(NameEx()));
        }

//        QString kolodka;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Ts");
    }

}

Ts::~Ts()
{

}

// чтение БД
bool Ts::ReadBd (QString& dbpath, Logger& logger)
{
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
                    new Ts(query, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Ts::ReadBd");
        return false;
    }

    logger.log("Сортировка списков ТС");
    Station::SortTs();

    return true;
}


// сформировать индекс сигнала по координатам
int Ts::getIndex(Logger& logger)
{
    // в конфигурациях МПЦ можем иметь свои модули ТС для контроля ОТУ
    // при этом сигналы Ebilock щписываются традиционно координатой  J (0...n-1)
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
    return index = (modul - 1) * tsPerModule + (_i - 1) * jmax + _j - 1;
}

int Ts::CompareByNames(const void* p1,const void* p2)
{
    return ((Ts*)p1)->Name() < ((Ts*)p2)->Name();
}

// проблемы реализации функций в  h-файле из-за перекрестных ссылок ts.h  и station.h
bool Ts::Sts      () { return st->TsSts      (index); }     // состояние (0/1), если мигает - 0
bool Ts::StsPulse () { return st->TsPulse    (index); }     // состояние мигания
bool Ts::StsRaw   () { return st->TsRaw      (index); }     // состояние ненормализованное
bool Ts::Sts_     () { return st->TsStsMoment(index); }     // состояние мгновенное
