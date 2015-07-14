#ifndef PEREGON_H
#define PEREGON_H

#include "station.h"

class Peregon : public SprBase
{
public:

    // открытые статические члены
    static QHash<int, Peregon*> Peregons;                   // хэш-таблица указателей на справочники перегонов

    // открытые статические функции
    static Peregon * GetById(int no);                       // получить справочник по номеру перегона
    static Peregon * Peregon::GetSprByOrgNoAndKrug (int no, int bridgeno);

    Peregon();
    ~Peregon();

private:

};

#endif // PEREGON_H
