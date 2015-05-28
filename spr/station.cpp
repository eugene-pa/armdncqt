#include <QVariant>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

#include "station.h"

QHash<int, Station*> Station::Stations;                     // хэш-таблица указателей на справочники станций

// чтение станций
Station::Station(QSqlQuery& query, Logger& logger)
{
    bool ret;

    tsStsRaw        = QBitArray(TsMaxLengthBits);           // инициируем битовые массивы
    tsStsPulse      = QBitArray(TsMaxLengthBits);
    tsStsRawPrv     = QBitArray(TsMaxLengthBits);
    tsStsPulsePrv   = QBitArray(TsMaxLengthBits);
    tsSts           = QBitArray(TsMaxLengthBits);

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
void Station::GetValue(QString& name, int& ret)
{
    ret = 0;

    Station * st;
    QString tsname;
    parseNames (name, st, tsname);

    QString stname = st == nullptr ? "" : st->name;

    bool pulse = false;
    if (tsname.indexOf("~")==0)
    {
        pulse = true;
        tsname = tsname.replace("~", "");
    }

    if (st != nullptr && Ts.contains(tsname))
    {
        class Ts * ts = Ts[tsname];
        ret =  ts->Locked() ? false :                         // Для блокированных ТС возвращаем false в выражениях (Решение не однозначное...)
               pulse ? ts->StsPulse() : ts->Sts();
    }
    else
    {
        qDebug() << QString("Ошибка идентификации сигнала %1").arg(name);
    }
}


// синтаксический разбор индексированных имен ТУ/ТС
// 1/3+
// 1/3+[#3]
// 1/3+[Минводы]
QRegularExpression RgxNameTsAndNoSt  (".+\\[#\\d+\\]");         // формат с номером станции ИМЯ[#НОМЕР_СТАНЦИИ]
QRegularExpression RgxNoSt           ("(?<=#)\\d+");            // НОМЕР_СТАНЦИИ в строке формата с номером станции
QRegularExpression RgxNameTs         ("[^[]+");                 // имя перед квадратными скобками
QRegularExpression RgxNameTsAndNameSt(".+\\[[^#].+\\]");        // формат с именем станции ИМЯ[СТАНЦИЯ]
QRegularExpression RgxNameSt         ("(?<=\\[).+?(?=\\])");    // СТАНЦИЯ

bool Station::parseNames (QString& srcname, Station*& st, QString& name)
{
    st = this;
    name = srcname;
//  KrugInfo krug = st.KrugObject;

    try
    {
        // 1. Проверка формата с номером ИМЯ[#НОМЕР_СТАНЦИИ]
        if (RgxNameTsAndNoSt.match(name).hasMatch())
        {
            QString snost = RgxNoSt.match(name).captured();
            int nost = snost.toInt();
            name = RgxNameTs.match(name).captured();
            st = GetByNo(nost);
        }
        else
            // 2. Проверка формата с именем станции
            if (RgxNameTsAndNameSt.match(name).hasMatch())
            {
                QString namest = RgxNameSt.match(name).captured();
                name = RgxNameTs.match(name).captured();
                st = GetByName(namest);
            }
    }
    catch (...)
    {
        qDebug() << QString("Ошибка парсинга выражения %1").arg(srcname);
        return false;
    }
    return true;
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
    logger.log(QString("Чтение таблицы [Stations] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Stations] WHERE NoSt > 0 ORDER BY [NoSt]");

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

// сортировака спимка TsSorted
void Station::sortTs()
{
    foreach (Station * st, Stations.values())
    {
        qSort(st->TsSorted.begin(), st->TsSorted.end(),Ts::CompareByNames);
    }
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
                maxI = 48;
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

// проверка бита в битовом массиве
bool Station::TestBit (QBitArray& bits, int index)
{
    return index >= 0 && index < bits.size() ? bits[index] : 0;
}

