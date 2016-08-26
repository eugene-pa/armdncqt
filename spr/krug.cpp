#include "krug.h"

std::vector<KrugInfo* > KrugInfo::Krugs;                    // список кругов, отсортированный по именам
std::unordered_map<int, KrugInfo*> KrugInfo::KrugsById;     // словарь кругов по номерам


KrugInfo::KrugInfo()
{

}

KrugInfo::~KrugInfo()
{

}

