#include "sysinfo.h"

SysInfo::SysInfo()
{
    st = nullptr;
    src.fill(0,SysInfoLength);
}

SysInfo::SysInfo(Station * pst)
{
    st = pst;
    src.fill(0,SysInfoLength);
}

SysInfo::~SysInfo()
{

}

// получить статус модулей БТ индексу 0-5
BYTE SysInfo::GetMtuMtsStatus(int i)
{
    switch (i)
    {
        case 0: return src[ 5]; break;
        case 1: return src[ 6]; break;
        case 2: return src[ 7]; break;
        case 3: return src[ 9]; break;
        case 4: return src[10]; break;
        case 5: return src[11]; break;
    }
    return 0;
}
