#ifndef ESR_H
#define ESR_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include "../common/logger.h"
#include "../spr/sprbase.h"

class Esr
{
public:
    // открытве статические функции
    static bool ReadBd (QString& dbpath, Logger& logger);   // чтение БД
    static QString NameByEsr(int esr);                      // получить имя по ЕСР
    static bool NameByEsr(int esr, QString& name);          // получить имя по ЕСР перегруженная
    static QString NameAndEsrByEsr(int esr);                // получить строку ИМЯ [ЕСР]
    static int EsrByDcName(QString name);                   // получить ЕСР по имени ДЦ

private:
   // закрытые члены - таблицы ЕСР
   static std::unordered_map <int, std::string> esrTable;   // ЕСР6 - имя станции общее
   static std::unordered_map <int, std::string> esr5Table;  // ЕСР5 - имя станции общее
   static std::unordered_map <int, std::string> esrDcTable; // ЕСР6 - имя станции в системе ДЦ
   static std::unordered_map <std::string, int> dcNameByEsr;// имя станции в системе ДЦ - ЕСР6

   QString buf;
};

#endif // ESR_H
