#include <QVariant>
#include "sprbase.h"
//#include "station.h"
#include "otu.h"

Otu::Otu(QSqlQuery& query, class KrugInfo* krug, Logger& logger)
{
    upok = false;
    no = no2 = 0;
    nost = nost2 = 0;
    st = st2 = nullptr;

    this->krug = krug;

    SetBaseType(BaseTu);
    bool ret;
    try
    {
        nost        = query.value("NoSt"     ).toInt(&ret); // номер станции
        no          = query.value("No"       ).toInt(&ret); // код команды
        shortName   = query.value("ShortName").toString();
        name        = query.value("Name"     ).toString();
        upok        = query.value("Upok"     ).toBool();
        if (upok)
        {
            nameTs1     = query.value("CheckTs1" ).toString();
            nameTs2     = query.value("CheckTs2" ).toString();
            epilogTu    = query.value("EpilogTu" ).toString();
            objectType  = query.value("ObjectType").toString();
            nost2       = query.value("NoSt2"    ).toInt(&ret); // номер станции сопряженной команды или 0
            no2         = query.value("No2"       ).toInt(&ret);// код сопряженной команды или 0
        }
        st          = Station::GetById(nost);
        if (nost2)
            st2 = Station::GetById(nost2);

        st->Otu[shortName.toStdString()] = this;
        st->OtuByNo[no] = this;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Otu");
    }

}

Otu::~Otu()
{

}


// чтение таблицы ОТУ [TuSpok]
bool Otu::ReadBd (QString& dbpath, class KrugInfo* krug, Logger& logger)
{
    logger.log(QString("Чтение таблицы [TuSpok] БД %1").arg(dbpath));

    try
    {
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
        if (GetSqliteBd(dbpath).open())
        {
            QString sql = "SELECT * FROM TuSpok ORDER BY NOST, NO";
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    new Otu(query, krug, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Otu::ReadBd");
        return false;
    }

    return true;
}

QString Otu::ToString()
{
    return QString("%1[#%2], ст.%3").arg(shortName).arg(no).arg(st->Name());
}
