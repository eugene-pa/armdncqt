// файл с глобальными переменными группы проектов QtArmDnc
#include "defines.h"

ProgrammType modulType;                                     // тип приложения

bool IsMonitor  () { return modulType==APP_MONITOR  ; }
bool IsArmTools () { return modulType==APP_ARMUTILS ; }
bool IsRss      () { return modulType==APP_MDMAGENT ; }
bool IsTablo    () { return modulType==APP_TABLO    ; }
