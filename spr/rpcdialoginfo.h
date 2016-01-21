#ifndef RPCDIALOGINFO_H
#define RPCDIALOGINFO_H

#include "../common/defines.h"
#include "../common/logger.h"


class rpcDialogInfo
{
public:
    rpcDialogInfo(UINT sddrBmEx, UINT tsGroups, UINT tsOffset);
    ~rpcDialogInfo();

    static bool Parse(QVector<rpcDialogInfo*>& list, QString& s,Logger& logger);// разбор строки
    static UINT sumLength(QVector<rpcDialogInfo*>& list);                       // подсчет общей длины данных по всем блокам

private:
    UINT addrBmEx;                                     // адрес БМ, включая ЕСР, имеющие место быть; заполняется при чтении INI файла
    UINT tsGroups;                                     // число групп ТС
    UINT tsOffset;                                     // смещение группы ТС

};

#endif // RPCDIALOGINFO_H
