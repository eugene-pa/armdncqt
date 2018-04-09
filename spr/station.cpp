#include <QVariant>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QtGlobal>

#include "krug.h"
#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"
#include "pereezd.h"
#include "esr.h"
#include "dstdatafromfonitor.h"
#include "abtcminfo.h"
#include "rpcdialoginfo.h"
#include "ecmpkinfo.h"

//#include "tu.h"

std::unordered_map<int, Station*> Station::Stations;        // хэш-таблица указателей на справочники станций
std::vector<Station*> Station::StationsOrg;                 // массив станций в порядке чтения из БД
bool    Station::LockLogicEnable;                           // включен логический контроль
bool    Station::InputStreamRss = false;                    // тип входного потока: InputStreamRss=true-Станция связи, false-Управление

short	Station::MainLineCPU;                               // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. основного канала связи
short	Station::RsrvLineCPU;                               // -1(3)/0/1/2 (отказ/откл/WAITING/OK) - сост. обводного канала связи
bool    Station::FastScanArchive;                           // быстрый просмотр архива - данные по объектам не обрабатываются

// конструктор принимает на входе запись из таблицы Stations
Station::Station(QSqlQuery& query, KrugInfo* krug, Logger& logger)
{
    bool ret;

    this->krug = krug;

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

    stsKpOk         = false;
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
    alarmATU        = false;

    errorLockLogicCount = 0;
    stsFrmMntrTsExpired = true;
    offsetDk        = 0;
    lenDk           = 0;

    mtsCount        = 0;
    mtuCount        = 0;

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
        krugId  = query.value("NoKrug").toInt(&ret);        // номер круга из БД
        version = query.value("Version").toInt(&ret);       // версия конфигурации станции (KpVersion)
        otuAddr = query.value("Config").toInt(&ret);        // адрес в подсистеме ОТУ (СПОК/БРОК)
        config  = query.value("ConfigKP2007").toString();   // строка конфигурации KP2007
        kpIdBase = query.value("GidUralKpId").toInt(&ret);  // ID КП, явно заданный в БД или 0
        enable  = query.value("OnOff").toBool();            // OnOff
        du      = query.value("DuAu").toBool();             // станция ДУ
        stsAu = !du;

        bitmapIdRss = query.value("BitmapID").toInt(&ret);  //
        radioIdRss = query.value("RadioID").toInt(&ret);    //
        radioIdMnt = query.value("RadioMonitorID").toInt(&ret);//
        T[0] = query.value("T1").toInt(&ret);               // коэффициенты связи
        T[1] = query.value("T2").toInt(&ret);               //
        T[2] = query.value("T3").toInt(&ret);               //
        T[3] = query.value("T4").toInt(&ret);               //
        forms = query.value("NumOfView").toInt(&ret);       // число форм

        orient = query.value("Orient").toString().toUpper();// ориентация
        bybylogic = query.value("ByByLogic").toBool();      // логика удалений (У1,У2)
        esr       = Esr::EsrByDcName(name);                 // код ЕСР из БД по имени станции
        gidUralId = kpIdBase ? kpIdBase : esr * 10 + 1;     // идентификация в ГИД УРАЛ

        mpcEbilock = rpcMpcMPK = rpcDialog = false;

        extForms = query.value("ExtFormList").toString();   // доп.формы, формат: 'имя_без_расширения' RADIOID ['имя_без_расширения' RADIOID]...
        typeEC = query.value("ObjectType").toString();      // тип ЭЦ

        ParseExtForms();                                    // разбор строки

        ParseConfigKP2007(logger);                          // разбор конфигурации


        Stations[no] = this;
        StationsOrg.push_back(this);
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

    if (st != nullptr && st->Ts.count(tsname.toStdString()))
    {
        class Ts * ts = st->Ts[tsname.toStdString()];
        ret =  ts->Disabled() ? false :                         // Для блокированных ТС возвращаем false в выражениях (Решение не однозначное...)
               pulse ? ts->StsPulse() : ts->Sts();
    }
    else
    {
        // 2016.09.02. Обрабатываю неявные виртуальные сигналы МДМ1 и МДМ2
        if (name=="МДМ1")
        {
            ret = st->IsCom3On() ? 1 : 0;
        }
        else
        if (name=="МДМ2")
        {
            ret = st->IsCom4On() ? 1 : 0;
        }
        else
            qDebug() << QString("Ошибка идентификации сигнала %1").arg(name);
    }
}


// синтаксический разбор индексированных имен ТУ/ТС; вызывается при вычислении выражений
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
//    KrugInfo krug = st.KrugObject;

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
Station * Station::GetById(int no, class KrugInfo * krug)
{
    int id = krug==nullptr ? no : krug->key(no);
    return Stations.count(id ) ? Stations[id] : nullptr;
}

// поучить справочник по номеру станции
Station * Station::GetByName(QString stname)
{
    for (auto rec : Stations)
    {
        if (rec.second->name == stname)
            return rec.second;
    }
    return nullptr;
}

// чтение БД
bool Station::ReadBd (QString& dbpath, KrugInfo* krug, Logger& logger, QString param)
{
    // по умолчанию
    if (param.length()==0)
        param = "WHERE NoSt > 0 ORDER BY [NoSt]";

    logger.log(QString("Чтение таблицы [Stations] из БД %1").arg(dbpath));
    QString sql = "SELECT * FROM [Stations] " + param;

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
                    new Station(query, krug, logger);
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
    if (!Ts.count(ts->Name().toStdString()))
        Ts[ts->Name().toStdString()] = ts;
    else
        logger.log(QString("Дублирование имен ТС: %1").arg(ts->NameEx()));

    // таблица, индексированая по индексу ТС в поле ТС (порядковый номер 0...n-1)
    int index = ts->GetIndex();
    if (!TsIndexed.count(index))
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
    if (!TsByIndxTsName.count(index))
        TsByIndxTsName[index] = ts;
    else
    {
        logger.log(QString("Дублирование индексов имен ТС: %1").arg(ts->NameEx()));
    }

    // таблица ТС, отсортированная по имени ТС (сортируем после чтения всех ТС)
    //TsSorted.append(ts);
    TsSorted.push_back(ts);
}

// сортировака списка TsSorted
void Station::SortTs()
{
    for (auto rec : Stations)
    {
        Station * st = rec.second;
        std::sort(st->TsSorted.begin(), st->TsSorted.end(),Ts::CompareByNames);
    }
}

// сортировка спимка ТУ
void Station::SortTu()
{
    for (auto rec : Stations)
    {
        Station * st = rec.second;
        std::sort(st->TuSorted.begin(), st->TuSorted.end(),Tu::CompareByNames);
    }
}

void Station::countMTUMTS()
{
    // считаем модули ТС/ТУ
    for (int i=0; i<mts.count(); i++)
    {
        if (IsTsPresent(i))
            mtsCount++;
        if (IsTuPresent(i))
            mtuCount++;
    }

    // для КП2000 сдвигаем пометки МТС на число модулей ТУ
    if (Kp2000())
    {
        // просмотр с конца, чтобы не мшать просмотру внесением изменений
        for (int i=mts.count()-1; i>=0; i--)
            if (IsTsPresent(i))
                SetBit(mts, i + mtuCount);
        for (int i=0; i<mtu.count(); i++)
            if (IsTuPresent(i))
                SetBit(mts, i, false);
    }

}

// сдвинуть пометки МТС для КП2000 на число ТУ
void Station::CountMT()
{
    for (auto rec : Stations)
    {
        rec.second->countMTUMTS();
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
        maxI = 1;
        maxJ = 4096;
        tsPerModule = 4096;
    }
    else
    if (rpcDialog)
    {
        // нюанс: при описании ст.РПЦ Диалог, например, ст.Новообразцовое, сиртуальные сигналы описываются начиная с 70-й строки
        //        без изменения номера; то есть допустимая размерность > 64
        maxModul = 2;
        maxI = 64 * 2;
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
    if (!Tu.count(tu->Name().toStdString()))
        Tu[tu->Name().toStdString()] = tu;
    else
        logger.log(QString("Дублирование имен ТУ: %1").arg(tu->NameEx()));

    TuSorted.push_back(tu);

    ushort ij = tu->IJ();
    class Tu * prev;
    if (TuByIJ.count(ij))
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
    if (allrc.count(obj->No()))
        logger.log(QString("Создание дублирующего объекта РЦ: %1").arg(obj->ToString()));
    else
        allrc[obj->No()] = obj;
}

// добавить СВТФ
void Station::AddSvtf(class Svtf * obj, Logger& logger)
{
    if (allsvtf.count(obj->No()))
        logger.log(QString("Создание дублирующего объекта СВТФ: %1").arg(obj->ToString()));
    else
        allsvtf[obj->No()] = obj;
}

// добавить СТРД
void Station::AddStrl(class Strl * obj, Logger& logger)
{
    if (allstrl.count(obj->No()))
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
    return routes.count(no) ? routes[no] : nullptr;
}

// "разрешить" ссылки ПРОЛОГ/ЭПИЛОГ/ПОЛЮС
// учитывая особенности реализации НСИ, можно утверждать, что синтаксис ТУ ПРОЛОГ/ЭПИЛОГ/ПОЛЮС - односложный без индексации
// поэтому поиск ТУ выполняется по имени в хэш-таблицах Tu класса Station
void Station::ParsePrologEpilog(Logger& logger)
{
    for (auto rec : Stations)
    {
        Station * st = rec.second;

        for (auto rec : st->Tu)
        {
            class Tu * tu = rec.second;
            QString name = tu->Prolog();
            if (name.length() > 0)
            {
                if (st->Tu.count(name.toStdString()))
                    tu->SetProlog(st->Tu[name.toStdString()]);
                else
                    logger.log(QString("Не найдена ТУ пролога %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }

            name = tu->Epilog();
            if (name.length() > 0)
            {
                if (st->Tu.count(name.toStdString()))
                    tu->SetEpilog(st->Tu[name.toStdString()]);
                else
                    logger.log(QString("Не найдена ТУ эпилога %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }

            name = tu->Polus();
            if (name.length() > 0)
            {
                if (st->Tu.count(name.toStdString()))
                    tu->SetPolus(st->Tu[name.toStdString()]);
                else
                    logger.log(QString("Не найдена ТУ полюса %1 для ТУ %2").arg(name).arg(tu->NameEx()));
            }

            tu->setTuEnum();

            if (tu->extTuSrc.length())
            {
                QRegularExpression RgxTu ("(?<=[=,])[^ ^,]+");
                QRegularExpressionMatchIterator m = RgxTu.globalMatch(tu->extTuSrc);
                while (m.hasNext())
                {
                    QString tusrc = m.next().captured();
                    Station * sttu;
                    QString name;
                    st->parseNames (tusrc, sttu, name); // разбор индексированных имен ТУ/ТС
                    if (sttu->Tu.count(name.toStdString()))
                    {
                        tu->extTu.push_back(sttu->Tu[name.toStdString()]);
                    }
                }
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
        formList.push_back(new ShapeId(this, name, radioIdMnt));

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
                formList.push_back(new ShapeId(this, form, radioid));
            }
        }
    }
}

// получить ТС по индексу
Ts * Station::GetTsByIndex(int indx)
{
    return TsIndexed.count(indx) ? TsIndexed[indx] : nullptr;
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


// получить состояние сигнала по имени
bool Station::GetTsStsByName (std::string name)
{
    return Ts.count(name) ? Ts[name]->Sts() : false;
}

// получить состояние мигания сигнала по имени
bool Station::GetTsPulseStsByName (std::string name)
{
    return Ts.count(name) ? Ts[name]->StsPulse() : false;
}

// d0 - состояние, d1 - мигание
int Station::GetTsStsByNameEx (std::string name)
{
   return  GetTsStsByName (name) | ((GetTsPulseStsByName (name) << 1)!=0);
}


// проверка состояния связи в последнем цикле опроса
bool Station::IsLinkOk()
{
    return GetSysInfo(stsRsrv)->linestatus==0;
}

// проверка изменения состояния связи со станцией с пред.цикла опроса
bool Station::IsLinkStatusChanged()
{
    return IsLinkOk() == stsLinkOkPrv;
}

// проверка были ли изменения ТС с прошлого цикла опроса
bool Station::IsTsChanged()
{
    return !(tsStsRaw == tsStsRawPrv && tsStsPulse == tsStsPulsePrv);
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

// получить время последнего опроса нужного блока
QDateTime Station::GetLastTime (bool rsrv)
{
    return rsrv ? rsrvSysInfo->LastTime() : mainSysInfo->LastTime();
}

// найти светофор по имени; используется при парсинге справочника маршрутов
Svtf * Station::GetSvtfByName(QString& name)
{
    for(auto rec : allsvtf)
    {
        if (rec.second->Name() == name)
            return rec.second;
    }
    return nullptr;
}

// найти стрелку по имени сигнала с учетом возможности индексации
Strl * Station::GetStrlByName(QString& name, int& no)
{
    Station * st;
    QString tsname;
    parseNames (name, st, tsname);

    no = 0;
    for(auto rec : st->allstrl)
    {
        Strl * strl = rec.second;
        if (strl->plus != nullptr && strl->plus ->NameTs() == tsname)
        {
            no = strl->No(); return strl;
        }
        if (strl->minus!= nullptr && strl->minus->NameTs() == tsname)
        {
            no = - strl->No(); return strl;
        }
    }
    return nullptr;
}

// найти РЦ по имени[возможно, индексированному]
Rc * Station::GetRcByName  (QString& name)
{
    Station * st;
    QString tsname;
    parseNames (name, st, tsname);

    //no = 0;
    for (auto map : st->allrc)
    {
        if (map.second->Name()==tsname)
            return map.second;
    }
    return nullptr;
}

// поиск ТУ по имени с возможностью индексации станции ЧО[#3], ЧО[Вад]
Tu * Station::GetTuByName (QString& name)
{
    Station * st;
    QString tuname;
    parseNames (name, st, tuname);

    for (auto rec : Tu)
    {
        class Tu * tu = rec.second;
        if (tu->Name() == tuname)
            return tu;
    }
    return nullptr;
}

// очистка информации о состоянии РЦ и маршрутов перед приемом данных из потока
void Station::ClearRcAndRouteInfo()
{
    for (auto map : allrc)
    {
        map.second->actualRoute = nullptr;
        map.second->actualtrain = nullptr;
        map.second->stsPassed   = false;
    }

    for (auto map : routes)
    {
        map.second->sts = Route::PASSIVE;
    }
}


// проверка наличия в матрице ТС сигналов, не описанных в БД
bool Station::IsUndefinedTsPresrnt()
{
    bool ret = false;
    for (int i = 0; i < TsMaxLengthBits; i++)
    {
        // если 1 или мигает и это не диагональ матрицы
        if ((tsStsRaw[i] || tsStsPulse[i]) && !TsIndexed.count(i))
        {
            if (Kp2000())
            {
                int row = (i/8)%8;
                int col = i%8;
                ret = row != col;
            }
            else
                ret = true;
            if (ret)
                break;
        }
    }
    return ret;
}

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

    // подсчет числа модулей
    countMTUMTS();

    // явное опсание исключений ТУ для разных режимов управления
    ParseTuEclusion();
}


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

        // выделяем модули (одиночные '1' и диапазоны вида '2-8')
        // здесь и ранее можно было использовать QString.split(QRegExp)
        QRegularExpressionMatchIterator match = QRegularExpression("\\d+[-\\d]*").globalMatch(modules);
        int nModules = 0;
        while (match.hasNext())
        {
            QString pare = match.next().captured().replace("-"," ");
            int m=0, mend=0, max = MaxModule;
            QTextStream(&pare) >> m >> mend;
            if (m > 0 && m <= max )
            do
            {
                if (m > 24)
                    mvv2present = true;
                SetBit (tu ? mtu : mts, m-1, true);            // пометить
                m++;
                nModules++;
            }
                while (m <= qMin (mend, max));
        }

        if (n!=nModules)
        {
            // сообщение о некорректном описании модулей
        }
    }
}


// установить бит в заданном массиве в заданное состояние (по умолчанию в 1)
void Station::SetBit (QBitArray& bits, int indx, bool s)
{
    if (bits.count() > 0 && indx >=0 && indx < bits.count())
        bits[indx] = s;
}


// TODO:
// --------------------------------------------------------------------------------------------------------

// есть ли неквитированные сообщения подсистемы логич.контроля
bool Station::IsNewErrorLockMsgPresent()
{
    // TODO: реализовать функцию
    return false;
}


// явное описание исключений ТУ для разных режимов управления
void Station::ParseTuEclusion()
{

}

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


// устарели ли ТС
bool Station::IsTsExpire()
{
    // TODO: реализовать функцию
    return stsFrmMntrTsExpired;
}

// обработка данных
// (функция SpellTS оригинального проекта)
// Надо уметь различать первичную обработку потока данных от станции связи (модуль Управление или аналоги)
// и вторичную обработку уже обработанного потока двнных (модуль АРМ ШН, Табло и др)
// Вместо использования определений препроцессора "MONITOR" и т.п. использую статическую переменную режиа опроса bRawData;
// если находимя в режиме поиска событий в архиве - можно не обрабатывать данные
void Station::AcceptTS   ()
{
    if (FastScanArchive)
        return;
    lastAcceptedTsTime = QDateTime::currentDateTime();      // засечка времени опроса
    errorLockLogicCount = 0;                                // сброс ошибок лог.контроля (посчитаем заново)

    // если входной поток от РСС - обрабатываем первичные данные
    if (InputStreamRss)
    {
        tsSts = tsStsRaw ^ tsInverse;                       // нормализация ТС

        GetVirtualTs();                                     // вычисление виртуальных ТС по формулам - 2 раза,
        GetVirtualTs();                                     // для обеспечения ссылок 1-го уровня на вирт.сигналы

        // CheckPeregons();                                    // контроль смежных перегонов
    }

    CheckTs     ();                                         // проверка ТС на достоверность
    CheckMode	();                                         // отследить режимы управлеия станцией
    CheckK7     ();											// отследить состояние К7

    alarmATU = this->Kp2007() ? IsRsrv() : GetTsStsByName("АТУ"); // ошибки АТУ

    CheckKp     ();                                         // состояние КП

    Strl::AcceptTS (this);									// состояние стрелок
    Rc  ::AcceptTS (this);                                  // состояние РЦ
    Svtf::AcceptTS (this);									// состояние светофоров	2014.10.22 перенес ДО МАРШРУТОВ
    Pereezd::AcceptTS (this);

    //DPereezd	  ::AcceptTS (NoSt);											// 2015.01.22 переезды
    //DPeregon	  ::AcceptTS ();												// 2015.01.22 все перегоны

}

// вычисление виртуальных ТС по формулам
void Station::GetVirtualTs()
{

}

// проверка ТС на достоверность (диагональ, "лишние" ТС)
void Station::CheckTs()
{

}

// отследить режимы управлеия станцией
void Station::CheckMode	()
{

}


// проверка работоспособности КП
void Station::CheckKp()
{
    stsKpOk = true;
//return !Kp2007 || Info == null
//           ? true
//           : // если не 2007 - истина
//           !Info.ErrorMT() && // ош. МТУ/МТС

//           !ErrAtuMain && // ош.АТУ    МВВ1 или МВВ2  основ
//           !ErrKeyMain && // ош.ключа  МВВ1 или МВВ2  основ
//           !ErrOutMain && // ош.выхода МВВ1 или МВВ2  основ
//           !ErrAtuRsrv && // ош.АТУ    МВВ1 или МВВ2  резерв
//           !ErrKeyRsrv && // ош.ключа  МВВ1 или МВВ2  резерв
//           !ErrOutRsrv && // ош.выхода МВВ1 или МВВ2  резерв

//           Info.IsNetOk() && // есть связь
//           (
//               (!Info.RsrvOn && Info.BitLineOnRsrv) // основной, резерв готов
//               || (Info.RsrvOn && Info.IsNetOk()) // резерв, есть связь с основным
//           )
//    ;
//}
}


// отследить состояние К7
void Station::CheckK7     ()
{

}

/*

        // строго говоря, при медленном опросе станций желательно было бы разнести функции первичной обработки и вычисления сигналов ТС и функции логического контроля
        // так как при вызове DRailwaySwitch::AcceptTS состояние РЦ и светофоров остаются с предыдущего шага

        CheckIZS			();														// контроль состояния искусств.замыкания стрелок
        CheckAD				();														// перекрытие сигналов на автодействии

        CheckLockingKeys  ();														// 2015.02.06.

#ifdef _MONITOR
#ifdef VIRTUAL_DSP
        if (m_DspStationNo==0)
#endif // #ifdef VIRTUAL_DSP
        {
            DRoute		::DoRoutes (NoSt);											// отследить состояние маpшpутов
            DRoute		::SubstituteZmk (NoSt);										// Косвенное замыкание
        }

#endif // #ifdef _MONITOR


#ifdef _MONITOR
#ifdef VIRTUAL_DSP
        if (m_DspStationNo==0)
#endif // #ifdef VIRTUAL_DSP
        {
        DPeregon	::RemoveOldTrains  ();											// удалить устаревшие поезда на перегоне
        DPeregon	::ClearEmptyPeregon();											// очистить пустые перегоны

        CheckErrorLock();															// формирование вирт.сигнала "ЭЦ.ОШБ" по результатам проверки соответствия зависимостей ЭЦ и АБ

        DLightSignal  ::CheckOutgoingSvtf(NoSt);									// состояние выходных светофоров и поездов, готовых к отправлению
///		DListTrains::AllTrains.CheckClosedSvtf  (NoSt);								// состояние выходных светофоров и поездов по отправлению
        TryPutNextStrel();															// очередную стрелку - в очередь

        DRailChain	  ::Mdp();														// модель движения поездов
        }
#endif // #ifdef _MONITOR




    // За блок вынесена обработка событий, так как там есть запущенные тайм-ауты
    // Это не очень правильно. Можно было работать по таймеру
    CheckEvents		();						// СООБЩЕНИЯ


    #ifndef _TABLO
    #ifndef _ARM_TOOLS
    AfxGetMainWnd()->PostMessage(WM_USER_TS_ACCEPTED,NoSt);
    #endif // #ifndef _ARM_TOOLS
    #endif // #ifndef _TABLO


#ifdef EXT_TU_ARMDNC_ENABLE
    #ifdef _MONITOR
    CheckTuWithTs();
    #endif // #ifdef _MONITOR
#endif // #ifdef EXT_TU_ARMDNC_ENABLE
}

*/
