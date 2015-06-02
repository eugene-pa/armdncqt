#ifndef TU_H
#define TU_H

#include <QHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "../common/logger.h"
#include "sprbase.h"


class Tu : public SprBase
{
public:
    // открытве статические функции
    static bool ReadBd (QString& dbpath, Logger& logger);       // чтение БД
    static int CompareByNames(const void*,const void*);

// открытве функции
    Tu(QSqlQuery& query, Logger& logger);
    ~Tu();

    QString& NameEx();                                      // имя станции и ТУ

    int IdRc   () { return norc  ; }
    int IdSvtf () { return nosvtf; }
    int IdStrl () { return nostrl; }

private:
                                                            // статические данные из БД
    int     iname;                                          // ключ имени в таблице TsNames

    int     modul;                                          //
    int     _i;
    int     _j;
    int     idtu;                                           // номер команды в системах типа EBILOCK

    int     norc;
    int     nostrl;
    int     nosvtf;

    float   delay;                                          // время запитки, сек

    QString prolog;                                         // имя ТУ пролога
    QString epilog;                                         // имя ТУ эпилога
    QString polus;                                          // имя ТУ полюса
    Tu *    tuProlog;                                       // Справочник ТУ пролога или null
    Tu *    tuEpilog;                                       // Справочник ТУ эпилога или null
    Tu *    tuPolus;                                        // Справочник ТУ полюса или null

    bool   otu;                                             // ОТУ
    bool   Locked ;                                         // блокировка сигнала (выключен)

    int  _kolodka;                                          // колодка
    QString Kolodka;                                        // колодка

    QString Kontact;                                        // контакт
    int    KontactInt;                                      // контакт целочисленный
    void * userobj;                                         // пользовательский объект

    Tu * next;                                              // следующий справочник в списке объединенных по одним координатам ТУ
    Tu * prev;                                              // предыдущий справочник в списке объединенных по одним координатам ТУ

//    bool Parsed;                                            // сигнал идентифицирован по типу
};

#endif // TU_H
