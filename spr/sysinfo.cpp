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

