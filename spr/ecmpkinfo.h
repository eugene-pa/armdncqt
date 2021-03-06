#ifndef ECMPKINFO_H
#define ECMPKINFO_H

#include "../common/defines.h"
#include "../common/logger.h"


class ecMpkInfo
{
public:
    ecMpkInfo(UINT length, UINT offset);
    ~ecMpkInfo();

    static bool Parse(std::vector<ecMpkInfo*>& list, QString& s,Logger& logger);// разбор строки
    static UINT sumLength(std::vector<ecMpkInfo*>& list);                       // подсчет общей длины данных по всем блокам

private:
    UINT tsGroups;                                     // число групп ТС
    UINT tsOffset;                                     // смещение группы ТС

};

#endif // ECMPKINFO_H
