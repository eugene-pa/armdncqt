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
                expression = new BoolExpression(formula, st);
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
    logger.log(QString("Чтение таблицы ТS БД %1").arg(dbpath));
    QString sql("SELECT * FROM [TS] INNER JOIN TS_Name ON TS.Cod = TS_Name.Cod ORDER BY NOST,[Module],[I],[J]");

    try
    {
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "qsqlite");
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
