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

    //QString& NameEx();                                      // имя станции и ТУ

    void SetParsed() { parsed = true; }
    bool IsParsed () { return parsed; }
    Tu * Next() { return next; }                            // следующий в цепочке
    void SetNext(Tu * tu) { next = tu; }
    void SetPrev(Tu * tu) { prev = tu; }                    // предыдущий в цепочке
    ushort IJ() { return ij; }                              // получить сформированный IJ
private:
                                                            // статические данные из БД
// закрытые члены
    int     iname;                                          // ключ имени в таблице TsNames

    int     modul;                                          //
    int     _i;
    int     _j;
    ushort  ij;                                             // упакованные координаты IJ
    int     idtu;                                           // номер команды в системах типа EBILOCK

    float   delay;                                          // время запитки, сек

    QString prolog;                                         // имя ТУ пролога
    QString epilog;                                         // имя ТУ эпилога
    QString polus;                                          // имя ТУ полюса
    Tu *    tuProlog;                                       // Справочник ТУ пролога или null
    Tu *    tuEpilog;                                       // Справочник ТУ эпилога или null
    Tu *    tuPolus;                                        // Справочник ТУ полюса или null

    bool   otu;                                             // ОТУ
    bool   locked ;                                         // блокировка сигнала (выключен)

    int  _kolodka;                                          // колодка
    QString kolodka;                                        // колодка

    QString kontact;                                        // контакт
    int    KontactInt;                                      // контакт целочисленный
    void * userobj;                                         // пользовательский объект

    Tu * next;                                              // следующий справочник в списке объединенных по одним координатам ТУ
    Tu * prev;                                              // предыдущий справочник в списке объединенных по одним координатам ТУ

    void * userObj;                                         // пользовательский объект
    bool parsed;
    bool validIJ;                                           // валидность координат

// закрытые функции
    ushort GetIJ();                                         // вычислить IJ по координатам
};

#endif // TU_H
