#ifndef TS_H
#define TS_H

#include "station.h"

class Ts : public SprBase
{
    friend class Rc;
    friend class Strl;
    friend class Svtf;

public:

// открытве статические функции
static bool ReadBd (QString& dbpath, class KrugInfo* krug, Logger& logger);        // чтение БД
static int CompareByNames(const void*,const void*);

// открытве функции
    Ts(QSqlQuery& query, class KrugInfo * krug, Logger& logger);
    ~Ts();

    //QString& NameEx();                                    // имя станции и ТС
    int GetIndex() { return index; }                        // индекс бита в поле ТС (0...n-1)
    int GetIndexOfName() { return iname; }                  // ключ имени в таблице TsNames
    Ts * GetNext() { return next; }                         // получить след.в цепочке
    void SetNext (Ts * ts) { next = ts; }                   // установить ссылку на след.в цепочке

    // значения ТС в итоге получаются обращением к битовым массивам в классе Station
    bool Sts      ();                                       // состояние (0/1), если мигает - 0
    bool StsPulse ();                                       // состояние мигания
    bool StsRaw   ();                                       // состояние ненормализованное
    bool Sts_     ();                                       // состояние мгновенное
    bool Value    ();                                       // 0/1 c учетом типа: для мигающих, если мигает, для позиционных 0/1

    //bool    IsBusy() { return busy;   }                     // поле Occupation
    //bool    IsSvtfmain() { return svtfmain; }               // поле SvtfMain

    void SetParsed() { parsed = true; }
    bool IsParsed () { return parsed; }

    bool IsVirtual () { return expression!=nullptr;  }
    bool IsReal    () { return kontact.length() > 0; }
    bool IsInverse () { return inverse; }
    bool IsPulsing () { return pulse;   }

    QString Kolodka() { return kolodka; }
    QString Kontact() { return kontact; }
    int M() { return modul; }
    int I() { return _i; }
    int J() { return _j; }

    QString GetTooltip();
    QString Place() { return place; }

private:
                                                            // статические данные из БД
    int     iname;                                          // ключ имени в таблице TsNames
    int     index;                                          // индекс бита в поле ТС (0...n-1)
    int     modul;                                          // поле БД модуль
    int     _i;                                             // поле БД I
    int     _j;                                             // поле БД J
    bool   inverse;
    bool   pulse;                                           // мигающий
    int    stativ;
    //int    place;
    int    _kolodka;
    QString kolodka;
    QString kontact;
    QString place;                                          // KNN:MM
    QString question;                                       // поле question

// выношу специфические свойства в классы объектов
//    bool   busy;
//    bool   svtfmain;
//    QString svtfdiag;                                       // тип диагностики
//    QString svtftype;                                       // тип: ВХ/ВЫХ/МРШ/ПРХ/МНВ/ПРС
//    QString svtferror;                                      // логич.выражение - контроль аварии светофора
//    QString strlzsname;
//    QString strlmuname;

    QString formula;                                        // выражение, описывающее вирт.сигнал
    class BoolExpression * expression;                      // указатель на BoolExpression, если есть формула

    Ts *   next;                                            // указатель на следующий ТС в цепочке ТС, находящихся в одной позиции по ключу INDEX
                                                            // QT содержит класс QMultiHash, однако я хочу явно видеть совмещенные ТС, поэтому использую свой механизм
    void * userObj;                                         // пользовательский объект
    bool parsed;
    bool validIJ;                                           // валидность координат

    static QString buf;                                     // строка для формирования сообщений


    // закрытые функции
    int  getIndex(Logger& logger);                          // сформировать индекс сигнала по координатам
};

#endif // TS_H
