#include <QVariant>
#include "esr.h"

QHash <int, QString> Esr::esrTable;                         // ЕСР6 - имя станции общее
QHash <int, QString> Esr::esr5Table;                        // ЕСР5 - имя станции общее
QHash <int, QString> Esr::esrDcTable;                       // ЕСР6 - имя станции в системе ДЦ
QHash <QString, int> Esr::dcNameByEsr;                      // имя станции в системе ДЦ - ЕСР6


// чтение БД
bool Esr::ReadBd (QString& dbpath, Logger& logger)
{
    bool ret;

    logger.log(QString("Чтение таблицы [Esr] из БД %1").arg(dbpath));
    QString sql("SELECT * FROM [Esr] ORDER BY Esr6");

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
                    // ЕСР6 - имя станции общее
                    int esr6 = query.value("Esr6").toInt(&ret);
                    QString name = query.value("Name").toString();
                    esrTable[esr6] = name;

                    // ЕСР6 - имя станции общее
                    int esr5 = esr6/10;
                    if (!esr5Table.contains(esr5))
                        esr5Table[esr5] = name;
                    else
                    {
                        esr5Table[esr5] += " / " + name;
                        logger.log(QString("Совпадающие ЕСР5: %1").arg(esr5Table[esr5]));
                    }

                    // ЕСР6 - имя станции в системе ДЦ
                    name.clear();
                    name = query.value("Alias").toString();

                    if (name.length())
                    {
                        if (!esrDcTable.contains(esr6))
                            esrDcTable[esr6] = name;
                        else
                            logger.log(QString("Совпадение ключей ЕСР - Alias: %1 - %2,%3").arg(esr6).arg(name).arg(esrDcTable[esr6]));

                        if (!dcNameByEsr.contains(name))
                            dcNameByEsr[name] = esr6;
                        else
                            logger.log(QString("Совпадение ключей Alias - ЕСР: %1 - %2,%3").arg(name).arg(esr6).arg(dcNameByEsr[name]));
                    }
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


// получить имя по ЕСР
// Если есть ДЦ-имя    - возвращает его, иначе
// Если есть общее имя - возвращает его, инаяе
//                       возвращает код ЕСР
QString Esr::NameByEsr(int esr)
{
    QString name;
    NameByEsr(esr, name);
    return name;
}

// получить строку ИМЯ [ЕСР]
QString Esr::NameAndEsrByEsr(int esr)
{
    QString name;
    if (NameByEsr(esr,name))
        return name;
    return QString("%1 [%2]").arg(name).arg(esr);
}

// получить имя по ЕСР перегруженная
bool Esr::NameByEsr(int esr, QString& name)
{
    name.clear();
    if (esrDcTable.contains(esr))
        name = esrDcTable[esr];
    else
    if (esrTable.contains(esr))
        name = esrTable[esr];
    else
    if (esr5Table.contains(esr))
        name = esr5Table[esr];
    else
        return false;
    return true;
}

// получить ЕСР по имени ДЦ
int Esr::EsrByDcName(QString name)
{
    return dcNameByEsr.contains(name) ? dcNameByEsr[name] : 0;
}
