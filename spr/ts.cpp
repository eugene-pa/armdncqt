#include <QVariant>
#include "sprbase.h"
#include "station.h"
#include "ts.h"
#include "../common/boolexpression.h"

QString Ts::buf;

Ts::Ts(QSqlQuery& query, Logger& logger)
{
    bool ret;
    try
    {
        //iname   = query.value("TS.Cod"  ).toInt(&ret);      // код имени
        iname   = query.value("Cod"  ).toInt(&ret);      // код имени

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
        nosftf  = query.value("NoSvtf"  ).toInt(&ret);
        locked  = query.value("Lock"    ).toBool();
        inverse = query.value("Inverse" ).toBool();
        busy    = query.value("Occupation").toBool();
        pulse   = query.value("Pulse"   ).toBool();
        svtfmain= query.value("SvtfMain").toBool();
        question= query.value("Question").toString();
        formula = query.value("ExtData" ).toString();

        next = nullptr;

        st = Station::GetByNo(nost);
        if (st != nullptr)
        {
            Ts::getIndex();
            if (formula.length())
            {
                // создаем класс для вычисления выражения и связываем его со слотом GetValue класса Station
formula = "&1";
                expression = new BoolExpression(formula);
                if (expression->Valid())
                    QObject::connect(expression, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
                else
                    logger.log(QString("%1. Ошибка синтаксиса в поле ExtData '%2': %3").arg(NameEx()).arg(expression->Source()).arg(expression->ErrorText()));

                //expression->GetValue();
            }
            st->AddTs(this, logger);
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
        bool exist = false;
        QSqlDatabase dbSql = (exist = QSqlDatabase::contains(dbpath)) ? QSqlDatabase::database(dbpath) :
                                                                        QSqlDatabase::addDatabase("QSQLITE", dbpath);
        if (!exist)
            dbSql.setDatabaseName(dbpath);
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
        logger.log("Исключение в функции Station::ReadBd");
        return false;
    }

    logger.log("Сортировка списков ТС");
    Station::sortTs();

    return true;
}

// Имя станции и ТС
QString& Ts::NameEx()
{
    return buf = QString("%1 ТС='%2'").arg(StMessage()).arg(name);
}

// сформировать индекс сигнала по координатам
int Ts::getIndex()
{
    if (st->IsMpcEbilock())
    {
        modul = 1;                                          //
        _i = 1;
    }

    int mmax = 0, imax = 0, jmax = 0, page = 0;
    st->GetTsParams(mmax, imax, jmax, page);
    return index = (modul - 1) * page + (_i - 1) * jmax + _j - 1;
}

int Ts::CompareByNames(const void* p1,const void* p2)
{
    return ((Ts*)p1)->Name() < ((Ts*)p2)->Name();
}
bool Ts::Sts      () { return st->TsSts      (index); }     // состояние (0/1), если мигает - 0
bool Ts::StsPulse () { return st->TsPulse    (index); }     // состояние мигания
bool Ts::StsDir   () { return st->TsDir      (index); }     // состояние ненормализованное
bool Ts::Stsmoment() { return st->TsStsMoment(index); }     // состояние мгновенное
