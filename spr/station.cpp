#include <QVariant>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QtGlobal>

#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"
#include "esr.h"
#include "dstdatafromfonitor.h"
#include "abtcminfo.h"
#include "rpcdialoginfo.h"
#include "ecmpkinfo.h"

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
    dirInputDk      .fill(0,TsMaxLengthBits);
    mts             .fill(0,MaxModule);                     // модули ТС
    mtu             .fill(0,MaxModule);                     // модули ТУ

    mvv2present     = false;
    mpcEbilock      = false;
    rpcMpcMPK       = false;
    rpcDialog       = false;
    apkdk           = false;
    abtcm           = false;
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
    offsetDk        = 0;
    lenDk           = 0;

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

        ParseConfigKP2007(logger);                          // разбор конфигурации

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

// получить состояние сигнала по имени
bool Station::GetTsStsByName (QString name)
{
    return Ts.contains(name) ? Ts[name]->Sts() : false;
}

// поддерживается ли расширенный блок диагностики
bool Station::IsSupportKpExt (bool rsrv)
{
    if (this->Kp2000())
        return false;
    int ver = (rsrv ? rsrvSysInfo : mainSysInfo)->LoVersionNo();
    return ver >= 21 && ver < 50;
}

// проверка срабатывания АТУ (отличается в разных версиях)
bool Station::IsAtuError (bool rsrv)
{
    if (this->Kp2000())
        return GetTsStsByName ("АТУ");
    SysInfo * p = rsrv ? rsrvSysInfo : mainSysInfo;
    return p->ErrAtu1() | p->ErrAtu2();
}

// проверка ошибки ключа     (отличается в разных версиях)
bool Station::IsKeyError (bool rsrv)
{
    if (this->Kp2000())
        return false;
    SysInfo * p = rsrv ? rsrvSysInfo : mainSysInfo;
    return p->ErrKey1() | p->ErrKey2();
}

// проверка ошибки выхода    (отличается в разных версиях)
bool Station::IsOutError (bool rsrv)
{
    if (this->Kp2000())
        return false;
    SysInfo * p = rsrv ? rsrvSysInfo : mainSysInfo;
    return p->ErrOut1() | p->ErrOut2();
}

// проверка выполнения теста
bool Station::IsTestMode (bool rsrv)
{
    if (this->Kp2000())
        return false;
    return (rsrv ? rsrvSysInfo : mainSysInfo)->IsTestMode();
}

// проверка включения сторожевого таймера
bool Station::IsWatchdogOn(bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->WatchDogOn() : false;
}

// проверка ошибки использования памяти
bool Station::IsMemError (bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->MemoryLeak() : false;
}

// проверка режима ретрансляции
bool Station::IsRetrans  (bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->RetranslateMode() : false;
}

// проверка подключения отладочной консоли
bool Station::IsConsol   (bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->DebugOtuMode() : false;
}

// проверка готовности системы ОТУ
bool Station::IsOtuLineOk    (bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->OtuLineOk() : false;
}

// проверка готовности УПОК/БРОК
//bool Station::IsOtuBrokOn    (bool rsrv)
//{
//    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->OtuBrokOn() : false;
//}


// проверка флага исполнения ОТУ
bool Station::IsOtuPending(bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->OtuBrokOn() : false;
}
// проверка включения режима отладки ОТУ
bool Station::IsDebugOtuMode(bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->DebugOtuMode() : false;
}

// проверка режима АРМ ДСП
bool Station::IsArmDspModeOn(bool rsrv)
{
    return IsSupportKpExt (rsrv) ? (rsrv ? rsrvSysInfo : mainSysInfo)->ArmDspModeOn() : false;
}

// TODO:
// --------------------------------------------------------------------------------------------------------


// ОПЦИИ:
// БРОК
// УПОК
// АДКСЦБ
// АПКДК
// АБТЦМ[(БУ12:N1,БУ13:N2)], N1, N2 - число байт по разным каналам
// EBILOCK[(N)], N - число информативных байт
// ЭЦ-МПК[(N1[,N2])] - число байт по разным каналам
// РПЦДИАЛОГ(БМАААААА:ГГ[,БМАААААА:ГГ]

void Station::ParseConfigKP2007(Logger& logger)
{
    //config = "МТУ=2(1,3) МТС=2(9-16) EBILOCK(63) АБТЦМ(БУ12:27,БУ13:26) АДКСЦБ РПЦДИАЛОГ(БМ06361101:64,БМ06361102:64) ЭЦ-МПК(186,188)";

    // БРОК
    if (config.indexOf("БРОК") >=0 || config.indexOf("УПОК") >=0)
        upokOtu= true;                                      // "БРОК" или "УПОК", чтобы точно указать тип ОТУ на станции

    // АДКСЦБ
    if (config.indexOf("АДКСЦБ") >=0)
        adkScb	= true;

    // EBILOCK
    if (config.indexOf("EBILOCK")>=0)
    {
        mpcEbilock	= true;
        // пробуем прочитать длину N в опции: EBILOCK(N)
        QRegularExpression regexEx("[Ee][Bb][Ii][Ll][Oo][Cc][Kk]\\(\\d+\\)");
        QRegularExpressionMatch match = regexEx.match(config);
        if (match.hasMatch())
        {
            QRegularExpression regexN("\\d+");
            nEbilockTsLength = regexN.match(match.captured()).captured().toInt();
            offsetDk += nEbilockTsLength;
        }
    }

    // АПКДК
    if (config.indexOf("АПКДК") >=0)
        apkdk = true;

    // АБТЦ
    abtcm = AbtcmInfo::Parse(abtcms, config, logger);

    // РПЦДИАЛОГ
    rpcDialog = rpcDialogInfo::Parse(rpcDialogs, config, logger);

    // ЭЦ-МПК
    rpcMpcMPK = ecMpkInfo::Parse(ecMpks, config, logger);

    // МТС,МТУ
    ParseMT (false);
    ParseMT (true);

/*

    // 2012.01.24. Читаем опции, задающие списки разрешенных команд через запятую: ДНЦ:СУ=  ДНЦ:РСУ  ДСП:ДУ
    // Списки должны храниться в списках объектов: 	EnableTuDncOnSuMode, EnableTuDncOnRsuMode, EnableTuDspOnDuMode
    // Присвоим значения по умолчанию
    sEnableTuDncOnSuMode	= "СУ.ОТ,ДОСУ,РОН,РОЧ,ОРОН,ОРОЧ,1РОН,1РОЧ,1ОРОН,1ОРОЧ,2РОН,2РОЧ,2ОРОН,2ОРОЧ";	// перечень ТУ, пропускаемых от АРМ ДНЦ в режиме СУ		ДНЦ:СУ=
    sEnableTuDncOnRsuMode	= "СУ.ОТ,ДОСУ";																	// перечень ТУ, пропускаемых от АРМ ДНЦ в режиме РСУ	ДНЦ:РСУ=
    sEnableTuDspOnDuMode	= "ВСУ.ВК,ВСУ.ОТ,РСТУ.ВК";														// перечень ТУ, пропускаемых от АРМ ДСП в режиме ДУ		ДСП:ДУ=
    sDisableTuDspOnSuMode	= "РОН,РОЧ,ОРОН,ОРОЧ,1РОН,1РОЧ,1ОРОН,1ОРОЧ,2РОН,2РОЧ,2ОРОН,2ОРОЧ,ДОСУ";			// перечень ТУ, запрещаемых  от АРМ ДСП в режиме СУ		-ДСП:СУ=
    sDisableTuDncOnDuMode	= "ВСУ.ВК,ВСУ.ОТ,РСТУ.ВК";														// перечень ТУ, запрещаемых  от АРМ ДНЦ в режиме ДЦ		-ДНЦ:ДУ=

    indx = s.Find("	ДНЦ:СУ=");							// ДНЦ:СУ=
    if (indx >= 0)
    {
        CStringEx sTmp = s.Mid(indx);
        sTmp.Replace("="," ");
        sTmp.GetToken();
        sEnableTuDncOnSuMode = sTmp.GetToken();
    }
    indx = s.Find("ДНЦ:РСУ=	");							// ДНЦ:РСУ=
    if (indx >= 0)
    {
        CStringEx sTmp = s.Mid(indx);
        sTmp.Replace("="," ");
        sTmp.GetToken();
        sEnableTuDncOnRsuMode = sTmp.GetToken();
    }
    indx = s.Find("ДСП:ДУ=");							// ДСП:ДУ=
    if (indx >= 0)
    {
        CStringEx sTmp = s.Mid(indx);
        sTmp.Replace("="," ");
        sTmp.GetToken();
        sEnableTuDspOnDuMode = sTmp.GetToken();
    }

    indx = s.Find("-ДСП:СУ=");							// -ДСП:СУ=
    if (indx >= 0)
    {
        CStringEx sTmp = s.Mid(indx);
        sTmp.Replace("="," ");
        sTmp.GetToken();
        sDisableTuDspOnSuMode = sTmp.GetToken();
    }

    indx = s.Find("-ДНЦ:ДУ=");							// -ДНЦ:ДУ=
    if (indx >= 0)
    {
        CStringEx sTmp = s.Mid(indx);
        sTmp.Replace("="," ");
        sTmp.GetToken();
        sDisableTuDncOnDuMode = sTmp.GetToken();
    }
*/
}

//#endif // #ifndef RAS_STATION
//#endif // #ifndef GID_URAL_PLUGIN
/*
void DStation::ParseModulesKP2007(CString& s,char cType)
{
    CStringEx sMdl(s);
    sMdl.Replace("("," ");
    sMdl.Replace(")"," ");
    sMdl.Replace("["," ");
    sMdl.Replace("]"," ");
    sMdl.Replace("="," ");
    sMdl.GetToken();					// МТС=
    CString sAll = sMdl.GetToken();		// число модулей

    int nAllDeclare = atoi(sAll);
    int nAll = 0;
    sMdl.Replace(","," ");				// запятые заменим на пробелы

    CStringEx sGroup;
    while ((sGroup = sMdl.GetToken()).GetLength())
    {
        CString sn;
        if (sGroup.Find("-") >=0)
        {
            sGroup.Replace("-"," ");
            sn = sGroup.GetToken();
            int n1 = atoi(sn);
            sn = sGroup.GetToken();
            int n2 = atoi(sn);
            for (int i=max(1,n1); i<= min (n2,MAX_MODULES_KP2007); i++)
            {
                modules2007[i-1] = cType;
                if (cType=='У')
                    nMtu2007 ++;
                else
                    nMts2007 ++;

                if (i>24)
                    bMvv2present = true;
                nAll++;
            }
        }
        else
        {
            int n = atoi(sGroup);
            if (n > 0 && n < MAX_MODULES_KP2007)
            {
                modules2007[n-1] = cType;
                if (cType=='У')
                    nMtu2007 ++;
                else
                    nMts2007 ++;

                if (n>24)
                    bMvv2present = true;
            }
            else
                PutLog(GetName(),s,"Ошибка описания модулей");
            nAll++;
        }
    }
    if (nAllDeclare != nAll)
        PutLog(GetName(),s,"Несоответствие указанного числа модулей описанию");
}

*/

// МТС=17(2-8,9-16,17-18)
// [Мм][Тт][Уу]
void Station::ParseMT (bool tu)
{
    QString prefix = tu ? "[МмMm][ТтTt][УуUu]=" : "[МмMm][TТтt][СсCc]=";
    // выделяем всю лексему с опцией
    QString lexem = QRegularExpression(prefix + "[^ ]*").match(config).captured();
    if (lexem.length())
    {
        // выделяем содержимое опции
        QString option = QRegularExpression("(?<=" + prefix + ")[^ ]+").match(lexem).captured();
        // определяем число модулей
        int n = QRegularExpression("\\d+(?=\\()").match(option).captured().toInt();
        // выделяем модули
        QString modules = QRegularExpression("(?<=[\\d+]\\()[^ \\)]+").match(option).captured();

        QRegularExpressionMatchIterator match = QRegularExpression("\\d+[-\\d]*").globalMatch(modules);
        while (match.hasNext())
        {
            QString pare = match.next().captured().replace("-"," ");
            int m1=0, m2=0, max = MaxModule;
            QTextStream(&pare) >> m1 >> m2;
            if (m1 > 0 && m1 <= max )
            do
            {
                if (m1 > 24)
                    mvv2present = true;
                SetBit (tu ? mtu : mts, m1-1, true);            // пометить
                m1++;
            }
                while (m1 <= qMin (m2, max));
        }
    }
}

// установить бит в заданном массиве в заданное состояние (по умолчанию в 1)
void Station::SetBit (QBitArray& bits, int indx, bool s)
{
    if (bits.count() > 0 && indx >=0 && indx < bits.count())
        bits[indx] = s;
}
