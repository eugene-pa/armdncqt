#include "sysinfo.h"

SysInfo::SysInfo()
{
    st = nullptr;
    src.fill(0,SysInfoLength);
    errors = 0;
}

SysInfo::SysInfo(Station * pst)
{
    st = pst;
    src.fill(0,SysInfoLength);
}

// принять заданное число байт статуса
void SysInfo::Parse(void *p, int l)
{
    for (int i=0; i < std::min((int)SysInfoLength, l); i++ )
        src[i] = ((BYTE *)p)[i];
    FixTime();
    linestatus = LineOK;
}


SysInfo::~SysInfo()
{

}

// получить статус линейки из 8 модулей БТ индексу 0-5
BYTE SysInfo::GetMtuMtsLineStatus(int i)
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

// установить статус линейки из 8 модулей БТ индексу 0-5
void SysInfo::SetMtuMtsLineStatus(int i, BYTE bte)
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


// проверка состояния модуля МТУ/МТС по индексу 0-47
bool SysInfo::MtuMtsStatus(int i)
{
    return i < 0 || i > MaxModule ? false : GetMtuMtsLineStatus(i/8) & (1 << i% 8);
}

// засечка времени опроса
void SysInfo::FixTime()
{
    tmdtPrev = tmdt;
    tmdt = QDateTime::currentDateTime();
}

// получить текст типа ошибки
QString SysInfo::ErrorType()
{
    switch (linestatus)
    {
        case LineTimeOut: return "Таймаут";
        case LineFormat : return "Формат";
        case LineCRC    : return "CRC";
        default         : return "-";
    }
}
