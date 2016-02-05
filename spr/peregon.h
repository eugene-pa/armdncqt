#ifndef PEREGON_H
#define PEREGON_H

#include "station.h"

class Peregon : public SprBase
{
public:

    // открытые статические члены
    static QHash<int, Peregon*> Peregons;                   // хэш-таблица указателей на справочники перегонов

    // открытые статические функции
    static Peregon * Peregon::GetById (int no, class KrugInfo* krug = nullptr);    // получить справочник по номеру перегона

    static void AcceptTS (class Station *);                 // обработка объектов по станции

    Peregon();
    ~Peregon();

private:

};

#endif // PEREGON_H
