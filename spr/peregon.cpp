#include "peregon.h"

QHash<int, Peregon*> Peregon::Peregons;              // хэш-таблица указателей на справочники перегонов

Peregon::Peregon()
{

}

Peregon::~Peregon()
{

}


// получить справочник по номеру перегона
Peregon * Peregon::GetById(int no)
{
    return Peregons.contains(no) ? Peregons[no] : nullptr;
}

// TODO: реализовать
Peregon * Peregon::GetSprByOrgNoAndKrug (int no, int bridgeno)
{
    return Peregon::GetById(no);
}
