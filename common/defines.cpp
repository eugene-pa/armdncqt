// файл с глобальными переменными группы проектов QtArmDnc
#include "defines.h"

ProgrammType modulType;                                     // тип приложения

bool IsMonitor  () { return modulType==APP_MONITOR  ; }
bool IsArmTools () { return modulType==APP_ARMUTILS ; }
bool IsRss      () { return modulType==APP_MDMAGENT ; }
bool IsTablo    () { return modulType==APP_TABLO    ; }

// глобальные переменные
bool g_ShowStrlText     = true;                             // отображение надписей на стрелках
bool g_ShowSvtfText     = true;                             // отображение надписей на светофорах
bool g_ShowClosedSvtf   = true;                             // отображение закрытых светофоров
bool g_ArchiveView;                                         // просмотр архива
bool g_bIgnoreServerTime = true;                            // глобальная переменная - флаг, запрещает синхронизацию часов и отрубает контроль
bool g_bNoSetServerTime  = true;                            // флаг ТОЛЬКО запрещает синхронизацию часов
time_t	g_TmDtServer;                                       // серверное время
time_t	g_DeltaTZ = 0;                                      // 2014.10.29.Разница времени удаленного сервера и настоящего АРМ ШН
int  g_RealStreamTsLength;                                  // реальная длина однобитного блока данных ТС в актуальном потоке ТС; с учетом двухбитной передачи длина удваивается
bool g_QuickSearching;                                      // флаг ускорееного сканирования входного потока
class Station * g_actualStation;                            // актуальная станция

QPixmap * g_green,
        * g_red,
        * g_yellow,
        * g_gray,
        * g_white,
        * g_cyan,

        * g_green_box,
        * g_green_box_tu,                           // МТУ ок
        * g_green_dark_box,
        * g_red_box,
        * g_red_box_tu,                             // МТУ error
        * g_red_dark_box,
        * g_yellow_box,
        * g_yellow_dark_box,
        * g_gray_box,
        * g_white_box,
        * g_green_box_blink;                                // индикаторы статуса
