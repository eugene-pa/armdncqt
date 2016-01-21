#ifndef ECMPKINFO_H
#define ECMPKINFO_H

#include "../common/defines.h"
#include "../common/logger.h"


class ecMpkInfo
{
public:
    ecMpkInfo(UINT length, UINT offset);
    ~ecMpkInfo();

    static bool Parse(QVector<ecMpkInfo*>& list, QString& s,Logger& logger);// разбор строки
    static UINT sumLength(QVector<ecMpkInfo*>& list);                       // подсчет общей длины данных по всем блокам

private:
    UINT tsGroups;                                     // число групп ТС
    UINT tsOffset;                                     // смещение группы ТС

};

#endif // ECMPKINFO_H
