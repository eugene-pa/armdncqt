#ifndef TU_H
#define TU_H

#include "station.h"

class Tu : public SprBase
{
public:
    // открытве статические функции
    static bool ReadBd (QString& dbpath, class KrugInfo* krug, Logger& logger);       // чтение БД
    static int CompareByNames(const void*,const void*);

// открытве функции
    Tu(QSqlQuery& query, class KrugInfo* krug, Logger& logger);
    ~Tu();

    //QString& NameEx();                                      // имя станции и ТУ

    void SetParsed() { parsed = true; }
    bool IsParsed () { return parsed; }
    Tu * Next() { return next; }                            // следующий в цепочке
    void SetNext(Tu * tu) { next = tu; }
    void SetPrev(Tu * tu) { prev = tu; }                    // предыдущий в цепочке
    ushort IJ() { return ij; }                              // получить сформированный IJ

    QString& Prolog() { return prolog; }                    // имя ТУ пролога
    QString& Epilog() { return epilog; }                    // имя ТУ эпилога
    QString& Polus () { return polus ; }                    // имя ТУ полюса

    void SetProlog(Tu * tu) { tuProlog = tu; }              // проставить пролог
    void SetEpilog(Tu * tu) { tuEpilog = tu; }              // проставить эпилог
    void SetPolus (Tu * tu) { tuPolus = tu; }               // проставить полюс

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

    Tu * next;                                              // следующий справочник в списке объединенных по одним координатам ТУ
    Tu * prev;                                              // предыдущий справочник в списке объединенных по одним координатам ТУ

    bool parsed;
    bool validIJ;                                           // валидность координат

// закрытые функции
    ushort GetIJ();                                         // вычислить IJ по координатам
};

#endif // TU_H
