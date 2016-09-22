#ifndef PEREEZD_H
#define PEREEZD_H

#include "station.h"
#include "../shapes/shapeprzd.h"

class Pereezd : public SprBase
{
    friend class DlgPereezd;

public:
    Pereezd(QSqlQuery&, class KrugInfo* , Logger&);         // конструктор
    ~Pereezd();

    static std::unordered_map<int,Pereezd*>  Pereezds;		// хэш-таблица указателей на справочники переездов, индексированная по ID переезда
    static bool ReadBd (QString&, class KrugInfo*, Logger&);// чтение БД
    static bool DbTablePresent;                             // таблица переездов в БД поддерживается!
    static void AcceptTS (class Station *);                 // обработка переездов по станции
    static Pereezd * findByShape (class ShapePrzd *);		// сопоставление заданного примитива с переездом из БД

    QString About() Q_DECL_OVERRIDE;
    int noPrg;												// номер перегона
    class Peregon * prg;                                    // справочник перегона

    class BoolExpression * openVal ;                        // выражение для контроля "открыт"
    class BoolExpression * alarmVal;                        // выражение для контроля "авария"
    class BoolExpression * zagrVal ;                        // выражение для контроля "заград.сигнализация"
    class BoolExpression * izvVal  ;                        // выражение для контроля "извещение"

    bool	side;                                           // горловина 0-ЧЕТ, 1 - НЕЧЕТ
    QString srcRc;                                          // список РЦ
    std::vector <class Rc*> rcList;							// список указателей на классы РЦ
    bool isOpen;											// открыт
    bool isAlarm;											// авария
    bool isZagr;											// заград.сигнализация
    bool isIzv;												// извещение
};

#endif // PEREEZD_H
