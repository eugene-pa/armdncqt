#include <QVariant>
#include <QSqlQuery>
#include "../common/defines.h"
#include "../common/logger.h"
#include "stationbase.h"

// облегченный класс-описатель станции для приложений, где нужна информация о станцийх,
// но не нужна полная информация об объекте

std::unordered_map<int, StationBase*> StationBase::Stations;// хэш-таблица указателей на справочники станций
std::vector<StationBase*> StationBase::StationsOrg;         // массив станций в порядке чтения из БД

// конструктор на базе записи в БД
StationBase::StationBase(QSqlQuery& query, int krug, Logger& logger)
{
    bool ret;
    this->krug = krug;
    try
    {
        no      = query.value("NoSt").toInt(&ret);          // номер станции
        name    = query.value("NameSt").toString();         // имя станции
        noext   = query.value("NoStGet").toString();        // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]

        ras     = query.value("Ras").toInt(&ret);           // номер станции связи
        addr    = query.value("Addr").toInt(&ret);          // линейный а дрес
        krugId  = query.value("NoKrug").toInt(&ret);        // номер круга из БД
        version = query.value("Version").toInt(&ret);       // версия конфигурации станции (KpVersion)
        otuAddr = query.value("Config").toInt(&ret);        // адрес в подсистеме ОТУ (СПОК/БРОК)
        config  = query.value("ConfigKP2007").toString();   // строка конфигурации KP2007
        kpIdBase = query.value("GidUralKpId").toInt(&ret);  // ID КП, явно заданный в БД или 0
        enable  = query.value("OnOff").toBool();            // OnOff
        du      = query.value("DuAu").toBool();             // станция ДУ

        bitmapIdRss = query.value("BitmapID").toInt(&ret);  //
        radioIdRss = query.value("RadioID").toInt(&ret);    //
        radioIdMnt = query.value("RadioMonitorID").toInt(&ret);//
        T[0] = query.value("T1").toInt(&ret);               // коэффициенты связи
        T[1] = query.value("T2").toInt(&ret);               //
        T[2] = query.value("T3").toInt(&ret);               //
        T[3] = query.value("T4").toInt(&ret);               //

        orient = query.value("Orient").toString().toUpper();// ориентация
//      esr       = Esr::EsrByDcName(name);                 // код ЕСР из БД по имени станции
        gidUralId = kpIdBase ? kpIdBase : esr * 10 + 1;     // идентификация в ГИД УРАЛ

        Stations[no] = this;
        StationsOrg.push_back(this);

    }
    catch(...)
    {
        logger.log("Исключение в конструкторе StationBase");
    }
}

StationBase::~StationBase()
{
    qDebug() << "~StationBase()";
}

// поучить справочник по номеру станции
StationBase * StationBase::GetById(int no, int krug)
{
    int id = (krug << 16) + no;
    return Stations.count(id ) ? Stations[id] : nullptr;
}

// поучить справочник по номеру станции
StationBase * StationBase::GetByName(QString stname)
{
    for (auto rec : Stations)
    {
        if (rec.second->name == stname)
            return rec.second;
    }
    return nullptr;
}

// чтение БД
bool StationBase::ReadBd (QString& dbpath, int krug, Logger& logger)
{
    QString dbalias = "НСИ";
    logger.log(QString("Чтение таблицы [Stations] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Stations] WHERE NoSt > 0 ORDER BY [NoSt]");

    try
    {
        QSqlDatabase db = QSqlDatabase::database(dbalias);
        if (!db.isOpen())
        {
            db = QSqlDatabase::addDatabase("QSQLITE", dbalias);
            db.setDatabaseName(dbpath);
            db.open();
        }
        if (db.isOpen())
        {
            QSqlQuery query(db);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    new StationBase(query, krug, logger);
                }
            }
        }
        else
        {
            logger.log("Проблема открытия БД " + dbpath);
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Station::ReadBd");
        return false;
    }

    return true;
}
