#include <QVariant>
#include "station.h"

QHash<int, Station*> Station::Stations;                     // хэш-таблица указателей на справочники станций

// чтение станций
Station::Station(QSqlQuery& query, Logger& logger)
{
    bool ret;
    try
    {
        no      = query.value("NoSt").toInt(&ret);          // номер станции
        name    = query.value("NameSt").toString();         // имя станции
        noext   = query.value("NoStGet").toString();        // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]

        ras     = query.value("Ras").toInt(&ret);           // номер станции связи
        addr    = query.value("Addr").toInt(&ret);          // линейный а дрес
        krugId  = query.value("NoKrug").toInt(&ret);        // номер круга
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
        forms = query.value("NumOfView").toInt(&ret);       // число форм
        extForms = query.value("ExtFormList").toString();   // доп.формы
        orient = query.value("Orient").toString();          // ориентация

//      esr     = // код ЕСР
        gidUralId = kpIdBase ? kpIdBase : esr * 10 + 1;     // идентификация в ГИД УРАЛ

        mpcEbilock = rpcMpcMPK = rpcDialog = false;

        Stations[no] = this;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Station");
    }
}

Station::~Station()
{

}

// вычисление переменной в выражении формата ИМЯ_ТС[ИМЯ_ИЛИ_#НОМЕР_СТАНЦИИ]
int Station::GetVar(QString& name)
{
    return 0;
}

// поучить справочник по номеру станции
Station * Station::GetByNo(int no)
{
    return Stations.contains(no) ? Stations[no] : nullptr;
}

// поучить справочник по номеру станции
Station * Station::GetByName(QString stname)
{
    foreach (Station * st, Stations.values())
    {
        if (st->name == stname)
            return st;
    }
    return nullptr;
}

// чтение БД
bool Station::ReadBd (QString& dbpath, Logger& logger)
{
    logger.log(QString("Чтение станций из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Stations] WHERE NoSt > 0 ORDER BY [NoSt]");

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
                    new Station(query, logger);
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

// добавить ТС
void Station::AddTs (class Ts* ts, Logger& logger)
{
    // таблица, индексированная по текстовому имени ТС
    if (!Ts.contains(ts->Name()))
        Ts[ts->Name()] = ts;
    else
        logger.log(QString("Дублирование имен ТС: %1").arg(ts->NameEx()));

    // таблица, индексированая по индексу ТС в поле ТС (порядковый номер 0...n-1)
    int index = ts->GetIndex();
    if (!TsIndexed.contains(index))
        TsIndexed[index] = ts;
    else
    {
        class Ts * prev = TsIndexed[index];
        for (int i=0; i<3; i++)
        {
            if (prev->GetNext() == nullptr)
                break;
            prev = prev->GetNext();
        }
        if (prev->GetNext() == nullptr)
        {
            prev->SetNext(ts);
            logger.log(QString("Сигнал %1 добавлен в цепочку за сигналом %2").arg(ts->NameEx()).arg(prev->Name()));
        }
        else
            logger.log(QString("Превышено оганичение на число совместных ТС в узле матрицы: %1").arg(ts->NameEx()));
    }

    // таблица, индексированая по индексу имени в таблице TsNames
    index = ts->GetIndexOfName();
    if (!TsByIndxTsName.contains(index))
        TsByIndxTsName[index] = ts;
    else
    {
        logger.log(QString("Дублирование индексов имен ТС: %1").arg(ts->NameEx()));
    }

    // таблица ТС, отсортированная по имени ТС (сортируем после чтения всех ТС)
    TsSorted.append(ts);
}

// Получить параметры позиционирования в матрице
// Эта функция требует уточнения и детализации
void Station::GetTsParams (int& maxModul, int& maxI, int& maxJ, int& tsPerModule)
{
    maxModul    = 4;
    maxI        = 24;
    maxJ        = 32;
    tsPerModule = 32;

    if (mpcEbilock)
    {
        maxModul = 1;
        maxI = 0;
        tsPerModule = 4096;
    }
    else
    if (rpcDialog)
    {
        maxModul = 2;
        maxI = 64;
        maxJ = 16;
        tsPerModule = 64 * 16;                              // 1024
    }
    else
    {
        switch (version)
        {
            case VERSION2007:
                tsPerModule = 32;
                break;
            case VERSIONDcmpk:
                maxModul = 1;
                maxI = 512;
                maxJ = 8;
//              tsPerModule = ;
                break;
            case VERSION2000:
                maxModul = 24;
                maxI = 8;
                maxJ = 8;
                tsPerModule = 64;
//              tsPerModule = 24 * 8 * 8;                   // 1536;
                break;
            case VERSIONRetime:
            case VERSIONKp2000Lomikont:
            case VERSION32X32:
                maxI = 32;
                maxJ = 32;
                tsPerModule = 1024;
                break;
        }
    }
}
