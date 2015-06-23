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

// установить статус модулей БТ индексу 0-5
void SysInfo::SetMtuMtsStatus(int i, BYTE bte)
{
    switch (i)
    {
        case 0: src[ 5] = bte; break;
        case 1: src[ 6] = bte; break;
        case 2: src[ 7] = bte; break;
        case 3: src[ 9] = bte; break;
        case 4: src[10] = bte; break;
        case 5: src[11] = bte; break;
    }
}
