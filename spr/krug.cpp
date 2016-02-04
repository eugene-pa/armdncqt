#include "krug.h"

QList<KrugInfo*     > KrugInfo::Krugs;                      // список кругов, отсортированный по именам
QHash<int, KrugInfo*> KrugInfo::KrugsById;                  // словарь кругов по номерам


KrugInfo::KrugInfo()
{

}

KrugInfo::~KrugInfo()
{

}

