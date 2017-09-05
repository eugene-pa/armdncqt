#ifndef RPCBM_H
#define RPCBM_H

#include <vector>
#include <../common/common.h>
#include "common/blockingrs.h"

// класс-оболочка одной БМ РПЦ

class RpcBM
{
public:
    static std::vector<RpcBM> allBM;            // вектор классов RpcBM; размер = числу БМ
    static int allGroups;                       // обще число групп на все БМ
    static void PollingLine(BlockingRS& rs);    // реализация опроса

    RpcBM(DWORD addr, int n);

private:
    DWORD   addrBM;                             // аддрес БМ
    int     groups;                             // число групп
    int     offset;                             // смещение в общем массиве данных для данной БМ
    time_t  tLast;                              // время последнего удачного сеанса связи
};

#endif // RPC_H
