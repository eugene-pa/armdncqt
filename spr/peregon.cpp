#include "peregon.h"
#include "krug.h"

std::unordered_map<int, Peregon*> Peregon::Peregons;              // хэш-таблица указателей на справочники перегонов

Peregon::Peregon()
{

}

Peregon::~Peregon()
{

}

// получить справочник по номеру перегона и кругу
Peregon * Peregon::GetById (int no, KrugInfo* krug )
{
    int id = krug==nullptr ? no : krug->key(no);
    return Peregons.count(id) ? Peregons[id] : nullptr;
}
