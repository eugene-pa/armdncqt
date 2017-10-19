#ifndef ABTCMINFO_H
#define ABTCMINFO_H

#include "../common/defines.h"
#include "../common/logger.h"

class AbtcmInfo
{
public:
    AbtcmInfo(UINT addr, UINT length, UINT offset);
    ~AbtcmInfo();

    static bool Parse(std::vector<AbtcmInfo*>& list, QString& s,Logger& logger); // разбор строки
    static UINT sumLength(std::vector<AbtcmInfo*>& list);       // подсчет общей длины данных по всем блокам

private:
    UINT    addr;                                           // адрес блока
    UINT    length;                                         // длина блока в байтах
    UINT    offset;                                         // смещение в целевом массиве
    UINT    count;                                          // статистика приема
    QDateTime arAbtcmLastDataRcvd;                          // последний прием по блоку

};

#endif // ABTCMINFO_H
