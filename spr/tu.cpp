#include <QVariant>
#include "sprbase.h"
#include "station.h"
//#include "ts.h"
//#include "tu.h"
//#include "../common/boolexpression.h"
//#include "rc.h"
//#include "strl.h"
//#include "svtf.h"


Tu::Tu(QSqlQuery& query, class KrugInfo* krug, Logger& logger)
{
    this->krug = krug;

    SetBaseType(BaseTu);
    bool ret;
    try
    {
        parsed = false;
        tuProlog = tuEpilog = tuPolus = next = prev =nullptr;

        iname   = query.value("Cod"     ).toInt(&ret);      // код имени ( в C# было "TU.Cod")
        name    = query.value("NameTu"  ).toString();       // имя ТY
        name    = query.value("NameTu"  ).toString();       // имя ТY
        nost    = query.value("NoSt"    ).toInt(&ret);      // номер станции
        modul   = query.value("Module"  ).toInt(&ret);
        _i      = query.value("I"       ).toInt(&ret);
        _j      = query.value("J"       ).toInt(&ret);
        norc    = query.value("NoRc"    ).toInt(&ret);
        nostrl  = query.value("NoStrl"  ).toInt(&ret);
        nosvtf  = query.value("NoSvtf"  ).toInt(&ret);
        _kolodka= query.value("Kolodka" ).toInt(&ret);
        kolodka = QString("K%1").arg(_kolodka);
        kontact = query.value("Klem"    ).toString();
        locked  = query.value("Lock"    ).toBool();
        delay   = query.value("Delay"   ).toFloat();

        // доступ к полям алиасов по именам в сложных запросах QSqlQuery не работает , поэтому обращаемся по индексам полей в запросе
                                                            // 0 - c.NameTU,
        prolog  = query.value(1).toString();                // 1 - p.NameTU,
        epilog  = query.value(2).toString();                // 2 - e.NameTU,
        polus   = query.value(3).toString();                // 3 - s.NameTu
        if (prolog.indexOf("-")==0 && prolog.length()==1) prolog.clear();
        if (epilog.indexOf("-")==0 && epilog.length()==1) epilog.clear();
        if (polus .indexOf("-")==0 && polus .length()==1) polus .clear();

        st = Station::GetById(nost);
        // поле ОТУ импользуется только для ДЦ МПК и Ретайм
        otu     = st->IsRpcMpcMPK() || st->Retime() ? query.value("OTU").toFloat() : false;

        ij = GetIJ();                                       // cформировать IJ

        if (st != nullptr)
        {
            if (validIJ)
            {
                // добавляем ТУ в справочники Tu, TuByIJ, TsSorted
                st->AddTu(this, logger);

                // РЦ
                if (norc > 0)
                    Rc::AddTu(query, this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoRc: %2").arg(NameEx()).arg(norc));

                // Светофор
                if (nosvtf > 0)
                    Svtf::AddTu(query, this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoSvtf: %2").arg(NameEx()).arg(nosvtf));

                // Стрелка
                if (nostrl > 0)
                    Strl::AddTu(query, this, logger);
                else
                if (norc < 0)
                    logger.log(QString("%1. Ошибка в поле NoStrl: %2").arg(NameEx()).arg(nostrl));

            }
            else
            {
                logger.log(QString("Ошибка описания m/i/j ТУ %1: %2/%3/%4").arg(NameEx()).arg(modul).arg(_i).arg(_j));
            }
        }
        else
        {
            logger.log(QString("Некорректный номер станции. Игнорируем ТУ %1").arg(NameEx()));
        }
    }
    catch(...)
    {
        logger.log("Исключение в конструкторе Tu");
    }

    kolodka = QString("K%1").arg(_kolodka);
    place = QString("%1:%2").arg(kolodka).arg(kontact);

}

Tu::~Tu()
{

}

// получить IJ
ushort Tu::GetIJ()
{
    // по умолчанию - КП2007, кодирование ТУ:
    // КП-2007:    TTTT.MMMM  MMOO.OOOO
    // TTTT - время исполнения ТУ, сек
    // MMMMMM - номер модуля 1-48 (макс. 63)
    // OOOOOO - номер выхода 1-32 (макс. 63)

    ushort ij = 0;
    validIJ = false;
    if (st)
    {
        if (st && st->IsMpcEbilock())                       // МПЦ:        №ТУ
        {
            validIJ = _j>0 && _j<=4096;
            ij = _j;
        }
        else
        if (st->Kp2007())
        {
            validIJ = _i>0 && _i<=48 && _j>0 && _j<=32;
            ij = _i << 6 | _j;                              // КП-2007:    TTTT.MMMM  MMOO.OOOO
        }
        else
        if (st && st->Kp2000())                             // КП-2000:    TTTM.MMMM  IIII.JJJJ
        {
            validIJ = modul>0 && modul<=25 &&_i>0 && _i<=8 && _j>0 && _j<=8;
            ij = modul << 8 | _i << 4 | _j;

            if (validIJ && st->Kp2000() && _kolodka > 0)
                st->MarkTu(modul);

        }
    }
    if (!validIJ)
        int a = 99;
    return ij;
}

// чтение таблицы [ТU]
bool Tu::ReadBd (QString& dbpath, class KrugInfo* krug, Logger& logger)
{
    logger.log(QString("Чтение таблицы [ТU] БД %1").arg(dbpath));

    try
    {
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
        if (GetSqliteBd(dbpath).open())
        {

            // ПРОБЛЕМА: если в таблице нет имени с кродом 0, запрос ниже выдает пустой набор данных
            // 1. пытаемся прочитать имя с нулевым индексом
            QSqlQuery queryTu0(dbSql);
            QString sql("SELECT * FROM TU_Name WHERE Cod=0");
            if (queryTu0.exec (sql) && !queryTu0.next())
            {
                logger.log("В таблице TU_Name отсутствует имя с нулевым индексом. Таблица TU не может быть корректно обработана");
            }

            // 2016.02.01. В версии QT 5.5 с чтением псевдонимов все ок
            // 2015.06.03. Вылезла проблема с доступом к именованым полям в формате с алиасами: query.value("p.Name_Ts"]
            //             Могу выбирать поля только по индексу
            //             Так как формат TU плывет в общем случае (поля Сетуни, ДЦ МПК), алиасы надо пустить вперед, чтобы гарантировать индекс
            //sql = "SELECT *, c.NameTU, p.NameTU, e.NameTU, s.NameTu FROM ((((TU INNER JOIN TU_Name AS c ON TU.Cod = c.Cod) INNER JOIN TU_Name AS p ON TU.Prolog = p.Cod) INNER JOIN TU_Name AS e ON TU.Epilog = e.Cod) INNER JOIN TU_Name AS s ON TU.Polus  = s.Cod)  ORDER BY NOST,[Module],[I],[J]";
            sql = "SELECT c.NameTU, p.NameTU, e.NameTU, s.NameTu,* FROM ((((TU INNER JOIN TU_Name AS c ON TU.Cod = c.Cod) INNER JOIN TU_Name AS p ON TU.Prolog = p.Cod) INNER JOIN TU_Name AS e ON TU.Epilog = e.Cod) INNER JOIN TU_Name AS s ON TU.Polus  = s.Cod)  ORDER BY NOST,[Module],[I],[J]";
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    new Tu(query, krug, logger);
                }
            }
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции Tu::ReadBd");
        return false;
    }

    // после чтения всех ТУ можно "разрешить" все ссылки ПРОЛОГ/ЭПИЛОГ/ПОЛЮС
    logger.log("Обработка ссылок ПРОЛОГ/ЭПИЛОГ/ПОЛЮС команд ТУ");
    Station::ParsePrologEpilog(logger);

    logger.log("Сортировка списка ТУ TuSorted");
    Station::SortTu();
    Station::CountMT();

    return true;

}

int Tu::CompareByNames(const void* p1,const void* p2)
{
    return ((Tu*)p1)->Name() < ((Tu*)p2)->Name();
}

// записать последовательность ТУ
void Tu::setTuEnum()
{
    tuEnum = polus + " " + prolog + + " " + (_kolodka > 0 ? (name  + " ") : "") + epilog;
}
