#include "peregon.h"
#include "krug.h"

std::unordered_map<int, Peregon*> Peregon::Peregons;              // хэш-таблица указателей на справочники перегонов

Peregon::Peregon(QSqlQuery& query, KrugInfo* krug, Logger& logger)
{
    bool ret;

    timeChangeDir = timeErBusy = timeErFree = timeErCompare = 0;
    bErBusyInformed = bErFreeInformed = bErCompareformed = false;
    sKzpOdd = sKzpEvn = sDir1 = sDir2 = dirActual = 0;
    noRcEvnFrom = noRcEvnTo = noRcOddFrom = noRcOddTo = 0;
    leftOddOrient = false;                                  // Нестандартная ориентация изображения станции (слева - нечетные поезда)
    chdkOn        = true;

    this->krug = krug;
    try
    {
        no      = query.value("NoPrg").toInt(&ret);         // номер перегона
        //name    = query.value("NameSt").toString();       // имя станции
        nostup  = query.value("NoStUp").toInt(&ret);        // номер станции сверху/слева
        nostdown= query.value("NoStDown").toInt(&ret);      // номер станции снизу/справа
        stup    = Station::GetById(nostup, krug);
        stdown  = Station::GetById(nostdown, krug);

        pathes  = query.value("Pathes").toInt(&ret);        // путей
        blind   = query.value("Pathes").toBool();           // слепой
        blindOddTime = query.value("BlindOdd").toInt(&ret); // время хода нечетное
        blindEvnTime = query.value("BlindEvn").toInt(&ret); // время хода четное

        name        = stup   == nullptr ? " --- " : stup->Name();
        name       += stdown == nullptr ? " -  --- " : " - " + stdown->Name();
        //shortname   = stup   == nullptr ? " --- " : stup->Name().left(4);
        //shortname  += stdown == nullptr ? " -  --- " : " - " + stdown->Name().left(4);
        shortname   = query.value("NicName").toString();
        busyOdd     = query.value("BuzyOdd").toString();    // Выражение занятости в нечетном направлении
        busyEvn     = query.value("BuzyEvn").toString();    // Выражение занятости в   четном направлении

        // по умолчанию перегон ориентирован как ЧН
        // перегон считается обратно ориентированным (НЧ), если хотя бы одна примыкающая станция НЧ
        leftOddOrient = (stup != nullptr && !stup->IsOrientEvnOdd()) || (stdown != nullptr && !stdown->IsOrientEvnOdd());

        int id = krug==nullptr ? no : krug->key(no);
        Peregons[id] = this;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Peregon");
    }

}

Peregon::~Peregon()
{

}

// получить справочник по номеру перегона и кругу
Peregon * Peregon::GetById (int no, KrugInfo* krug )
{
    int id = krug==nullptr ? no : krug->key(no);
    return Peregons.count(id) ? Peregons[id] : nullptr;
}


// чтение БД
bool Peregon::ReadBd (QString& dbpath, KrugInfo* krug, Logger& logger)
{
    logger.log(QString("Чтение таблицы [Peregon] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Peregon] WHERE NoPrg > 0 ORDER BY [NoPrg]");

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
                    new Peregon(query, krug, logger);
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
