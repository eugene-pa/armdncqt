#include <QVariant>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"
#include "esr.h"
#include "dstdatafromfonitor.h"
//#include "tu.h"

QHash<int, Station*> Station::Stations;                     // хэш-таблица указателей на справочники станций
bool    Station::LockLogicEnable;                           // включен логический контроль
short	Station::MainLineCPU;                               // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. основного канала связи
short	Station::RsrvLineCPU;                               // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. обводного канала связи

// конструктор принимает на входе запись из таблицы Stations
Station::Station(QSqlQuery& query, Logger& logger)
{
    bool ret;

    tsStsRaw        .fill(0,TsMaxLengthBits);               // инициируем битовые массивы  нужным размером
    tsStsPulse      .fill(0,TsMaxLengthBits);
    tsStsRawPrv     .fill(0,TsMaxLengthBits);
    tsStsPulsePrv   .fill(0,TsMaxLengthBits);
    tsSts           .fill(0,TsMaxLengthBits);
    tsInverse       .fill(0,TsMaxLengthBits);

    mpcEbilock      = false;
    rpcMpcMPK       = false;
    rpcDialog       = false;
    apkdk           = false;
    adkScb          = false;
    upokOtu         = false;

    stsActual       = false;
    stsAu           = false;
    stsSu           = false;
    stsRu           = false;
    stsMu           = false;
    stsOn           = false;
    stsRsrv         = false;
    stsCom3On       = false;
    stsCom4On       = false;
    stsBackChannel  = false;

    errorLockLogicCount = 0;

    mainSysInfo = new SysInfo();
    mainSysInfo->st = this;
    rsrvSysInfo = new SysInfo();
    rsrvSysInfo->st = this;

    tSpokSnd = tSpokRcv = 0;                                // время приема/передачи данных в СПОК

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

        orient = query.value("Orient").toString();          // ориентация
        bybylogic = query.value("ByByLogic").toBool();      // логика удалений (У1,У2)
        esr       = Esr::EsrByDcName(name);                 // код ЕСР из БД по имени станции
        gidUralId = kpIdBase ? kpIdBase : esr * 10 + 1;     // идентификация в ГИД УРАЛ

        mpcEbilock = rpcMpcMPK = rpcDialog = false;

        extForms = query.value("ExtFormList").toString();   // доп.формы, формат: 'имя_без_расширения' RADIOID ['имя_без_расширения' RADIOID]...
        ParseExtForms();                                    // разбор строки


        Stations[no] = this;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Station");
    }
}

Station::~Station()
{
    qDebug() << "~Station()";
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
            st = GetById(nost);
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
Station * Station::GetById(int no)
{
    return Stations.contains(no) ? Stations[no] : nullptr;
}

// TODO: реализовать
Station * Station::GetSprByNoOrgAndKrug(int no, int bridgeno)
{
    return GetById(no);
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
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
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

// сортировака списка TsSorted
void Station::SortTs()
{
    foreach (Station * st, Stations.values())
    {
        qSort(st->TsSorted.begin(), st->TsSorted.end(),Ts::CompareByNames);
    }
}

// сортировка спимка ТУ
void Station::SortTu()
{
    foreach (Station * st, Stations.values())
    {
        qSort(st->TuSorted.begin(), st->TuSorted.end(),Tu::CompareByNames);
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

// добавить ТУ
void Station::AddTu (class Tu* tu, Logger& logger)
{
    // таблица, индексированная по текстовому имени ТС
    if (!Tu.contains(tu->Name()))
        Tu[tu->Name()] = tu;
    else
        logger.log(QString("Дублирование имен ТУ: %1").arg(tu->NameEx()));

    TuSorted.append(tu);

    ushort ij = tu->IJ();
    class Tu * prev;
    if (TuByIJ.contains(ij))
    {
        prev = TuByIJ[ij];
        buf = QString("ТУ %1 добавлена в цепочку за ТУ %2").arg(tu->NameEx()).arg(prev->Name());

        while (prev->Next())
        {
            prev = prev->Next();
            buf += QString(", %1").arg(prev->Name());
        }
        logger.log(buf);

        prev->SetNext(tu);
        tu->SetPrev(prev);
    }
    else
    {
        TuByIJ[ij] = tu;
    }
}

// проверка бита в битовом массиве
bool Station::TestBit (QBitArray& bits, int index)
{
    return index >= 0 && index < bits.size() ? bits[index] : 0;
}

// добавить РЦ
void Station::AddRc(class Rc * obj, Logger& logger)
{
    if (allrc.contains(obj->No()))
        logger.log(QString("Создание дублирующего объекта РЦ: %1").arg(obj->ToString()));
    else
        allrc[obj->No()] = obj;
}

// добавить СВТФ
void Station::AddSvtf(class Svtf * obj, Logger& logger)
{
    if (allsvtf.contains(obj->No()))
        logger.log(QString("Создание дублирующего объекта СВТФ: %1").arg(obj->ToString()));
    else
        allsvtf[obj->No()] = obj;
}

// добавить СТРД
void Station::AddStrl(class Strl * obj, Logger& logger)
{
    if (allstrl.contains(obj->No()))
        logger.log(QString("Создание дублирующего объекта СТРЛ: %1").arg(obj->ToString()));
    else
        allstrl[obj->No()] = obj;
}

// добавить маршрут
void Station::AddRoute(Route* route)
{
    routes[route->No()] = route;
}

// получить маршрут по номеру маршрута на станции
Route * Station::GetRouteByNo(int no)
{
    return routes.contains(no) ? routes[no] : nullptr;
}

// "разрешить" ссылки ПРОЛОГ/ЭПИЛОГ/ПОЛЮС
// учитывая особенности реализации НСИ, можно утверждать, что синтаксис ТУ ПРОЛОГ/ЭПИЛОГ/ПОЛЮС - односложный без индексации
// поэтому поиск ТУ выполняется по имени в хэш-таблицах Tu класса Station
void Station::ParsePrologEpilog(Logger& logger)
{
    foreach (Station * st, Stations.values())
    {
        foreach (class Tu * tu, st->Tu.values())
        {
            QString name = tu->Prolog();
            if (name.length() > 0)
            {
                if (st->Tu.contains(name))
                    tu->SetProlog(st->Tu[name]);
                else
                    logger.log(QString("Не найдена ТУ пролога %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }

            name = tu->Epilog();
            if (name.length() > 0)
            {
                if (st->Tu.contains(name))
                    tu->SetEpilog(st->Tu[name]);
                else
                    logger.log(QString("Не найдена ТУ эпилога %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }

            name = tu->Polus();
            if (name.length() > 0)
            {
                if (st->Tu.contains(name))
                    tu->SetPolus(st->Tu[name]);
                else
                    logger.log(QString("Не найдена ТУ полюса %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }
        }
    }
}

// обработка доп форм
// формат: 'имя_без_расширения' RADIOID ['имя_без_расширения' RADIOID]...
// 'Кавказская-Б' 2102
// 'Армавир-IБ' 2109 'Армавир-IВ' 2110
// Для разбора используем регулярные выражения "с просмотром вперед"
void Station::ParseExtForms()
{
    if (forms > 0)
    {
        formList.append(new ShapeId(this, name, radioIdMnt));

        if (forms > 1)
        {
            QRegularExpression rgxPare("\\'[^\\']+'\\s+\\d+");
            QRegularExpressionMatchIterator m = rgxPare.globalMatch(extForms);
            while (m.hasNext())
            {
                QString pare = m.next().captured();
                QRegularExpression rgxNames("(?<=\\').+?(?=\\')");
                QRegularExpression rgxRadio("\\d+\\Z");

                QString form = rgxNames.match(pare).captured();
                int radioid  = rgxRadio.match(pare).captured().toInt();
                formList.append(new ShapeId(this, form, radioid));
            }
        }
    }
}

// получить ТС по индексу
Ts * Station::GetTsByIndex(int indx)
{
    return TsIndexed.contains(indx) ? TsIndexed[indx] : nullptr;
}

// получить состояние сигнала в марице ТС
bool Station::GetTsStsByIndex     (int indx)
{
    return indx >=0 && indx < tsSts.count() ? tsSts[indx] : false;
}

// получить состояние мигания сигнала в марице ТС
bool Station::GetTsPulseStsByIndex(int indx)
{
    return indx >=0 && indx < tsStsPulse.count() ? tsStsPulse[indx] : false;
}

// получить оригинальное состояние сигнала в марице ТС
bool Station::GetTsStsRawByIndex(int indx)
{
    return indx >=0 && indx < tsStsRaw.count() ? tsStsRaw[indx] : false;
}

void Station::MarkInverse(int index)
{
    tsInverse[index] = true;
}

// TODO:
// --------------------------------------------------------------------------------------------------------

// есть ли неквитированные сообщения подсистемы логич.контроля
bool Station::IsNewErrorLockMsgPresent()
{
    // TODO: реализовать функцию
    return false;
}

// устарели ли ТС
bool Station::IsTsExpire()
{
    // TODO: реализовать функцию
    return false;
}
