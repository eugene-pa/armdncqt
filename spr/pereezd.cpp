#include "pereezd.h"
#include "rc.h"


std::unordered_map<int,Pereezd*>  Pereezd::Pereezds;		// хэш-таблица указателей на справочники переездов, индексированная по ID переезда
bool Pereezd::DbTablePresent = false;                       // таблица переездов в БД поддерживается!

Pereezd::Pereezd(QSqlQuery& query, KrugInfo* krug, Logger& logger)
{
    bool ret;
    openVal = alarmVal = zagrVal = izvVal = nullptr;
    noPrg = 0;
    prg = nullptr;
    side = false;
    isOpen = isAlarm = isZagr = isIzv = false;
    try
    {
        no      = query.value("No").toInt(&ret);            // номер переезда
        nost    = query.value("NoSt").toInt(&ret);          // номер станции
        st = Station::GetById(nost, krug);
        name    = query.value("Name").toString();           // имя
        noPrg   = query.value("Peregon").toInt(&ret);       // номер перегона
        prg = Peregon::GetById(noPrg, krug);

        QString s = query.value("Side").toString();;
        side = s.indexOf("Ч")==0 ? 0 : 1;

        s       = query.value("StsOpen").toString();
        if (s.length())
        {
            openVal = new BoolExpression(s);
            if (openVal->Valid())
                QObject::connect(openVal, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        }

        s       = query.value("StsAlarm").toString();
        if (s.length())
        {
            alarmVal = new BoolExpression(s);
            if (alarmVal->Valid())
                QObject::connect(alarmVal, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        }

        s       = query.value("StsZagr").toString();
        if (s.length())
        {
            zagrVal = new BoolExpression(s);
            if (zagrVal->Valid())
                QObject::connect(zagrVal, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        }

        s       = query.value("StsIzv").toString();
        if (s.length())
        {
            izvVal = new BoolExpression(s);
            if (izvVal->Valid())
                QObject::connect(izvVal, SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        }

        srcRc = query.value("RC").toString();
        if (srcRc.length() > 0)
        {
            QStringList list = srcRc.split(QRegExp("[\\s,]+"));
            for (QString s : list)
            {
                Rc * rc = st->GetRcByName(s);
                if (rc != nullptr)
                    rcList.push_back(rc);
                else
                {
                    logger.log(QString("Переезд %1. Ошибка описания РЦ %2").arg(name).arg(s));
                }
            }
        }
        Pereezds[no] = this;
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Pereezd");
    }

}

Pereezd::~Pereezd()
{

}

// чтение БД
bool Pereezd::ReadBd (QString& dbpath, KrugInfo* krug, Logger& logger)
{
    logger.log(QString("Чтение таблицы [Pereezd] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Pereezds] WHERE No > 0 ORDER BY [No]");

    try
    {
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
        if (dbSql.open())
        {
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                DbTablePresent = true;

                while (query.next())
                {
                    new Pereezd(query, krug, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Pereezd::ReadBd");
        return false;
    }

    return true;
}

QString Pereezd::About()
{
    return name;

//    sInfo.Format("Ст. %s, переезд %s (#%d) \r\nПереезд открыт: %s\r\nАврия переезда: %s\r\nЗаград.сигнал: %s\r\nИзвещение: %s",
//					SprSt ? SprSt->GetName() : "?",
//					Name,
//					No,
//					StsOpenStr,
//					StsAlarmStr,
//					StsZagrStr,
//					StsIzvStr);

}

// сопоставление заданного примитива с переездом из БД
Pereezd * Pereezd::findByShape (ShapePrzd * shp)
{
    for (auto rec : Pereezds)
    {
        Pereezd * przd = rec.second;
        if (	(przd->st == shp->st))
        {
            QString s11 = przd->openVal  ? przd->openVal ->Source() : "",
                    s21 = przd->alarmVal ? przd->alarmVal->Source() : "",
                    s31 = przd->zagrVal  ? przd->zagrVal ->Source() : "",
                    s41 = przd->izvVal   ? przd->izvVal  ->Source() : "";
            QString s12 = shp->formulas[0] ? shp->formulas[0]->Source() : "",
                    s22 = shp->formulas[1] ? shp->formulas[1]->Source() : "",
                    s32 = shp->formulas[2] ? shp->formulas[2]->Source() : "",
                    s42 = shp->formulas[3] ? shp->formulas[3]->Source() : "",
                    s52 = shp->formulas[4] ? shp->formulas[3]->Source() : "";

            if (    s11.indexOf(s12) >= 0
                 && s21.indexOf(s22) >= 0
                 && s31.indexOf(s32) >= 0
                 && ( s41.indexOf(s42) >= 0 || s41.indexOf(s52)>=0)
                )
                return shp->pereerd = przd;
            else
                int a = 99;
        }
    }
    qDebug() << "Не найден переезд в БД: " << shp->Dump();
    return shp->pereerd = nullptr;
}

// обработка переездов по станции
void Pereezd::AcceptTS (class Station *)
{
    for (auto rec : Pereezds)
    {
        rec.second->acceptTS();
    }
}

void Pereezd::acceptTS ()
{
    isOpen  = openVal  ? openVal ->GetValue() : false;
    isAlarm = alarmVal ? alarmVal->GetValue() : false;
    isZagr  = zagrVal  ? zagrVal ->GetValue() : false;
    isIzv   = izvVal   ? izvVal  ->GetValue() : false;
}
