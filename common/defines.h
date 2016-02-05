#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsItem>
#include <QColor>
#include <time.h>
#include <QHash>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QBitArray>
#include <QVariant>
#include "QCloseEvent"
#include "QDateTime"
#include "QRegularExpression"

//#include <QWidget>
//#include <QGraphicsView>

// определения, облегчающие перенос кода с MSVC&MFC в QT
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned int    UINT;
#ifndef Q_OS_WIN
typedef const char *    LPCTSTR;
#endif

#define SIGNATURE 0xAA55
#define DUBL 2
#define MAX_TRAINS_ON_PER 5
#define TRACE(param) qDebug() << param

#define COLORREF QColor
#define BOOL bool
#ifndef FALSE                                               // для WINDOWS определения FALSE,TRUE даются в minwindef.h
#define FALSE false
#endif
#ifndef TRUE
#define TRUE  true
#endif
#define CString QString
#define CPen QPen
#define CBrush QBrush
#define CDC  QPainter
#define CFont QFont
#define CBitmap QBitmap
#define strstr(str,text) (str.indexOf(text)>=0)
#define strcmp(str1,str2) (str1==str2)
#define strlen(str) str.length()

#define byteof(p,i) *(((BYTE *)&p)+i)                       // получить i-й байт области по произвольному указателю

class DStation;

enum ProgrammType
{
    APP_MONITOR		= 1,
    APP_MDMAGENT	= 2,        // 2
    APP_TABLO		= 3,        // 3
    APP_ARMUTILS	= 4,        // 4
    APP_PROTOCOL	= 5,        // 5
    APP_ARM32		= 6,        // 6
    APP_ARMENERGY	= 7,        // 7
    APP_BRIDGEASOUP	= 8         // 8
};

enum LogSourcer
{
    SRC_UNDEFINED = 0,
    LOG_MDM_AGENT = 1,          // 1 - Станция связи
    LOG_NET_AGENT,              // 2 - Сетевой агент
    LOG_ARM,                    // 3 - АРМ
    LOG_MONITOR,                // 4 - Управляющая подсистема
    LOG_TS,                     // 5 - Подсистема контроля
    LOG_ASOUP,                  // 6 - Подсистема связи с АСОУП
    LOG_TEST,                   // 7 - Подсистема диагностики
    LOG_NET,                    // 8 - Сетевой протокол
    LOG_SPOK,                   // 9 - СПОК
    LOG_BD,                     // 10- БД
    LOG_SETUN,                  // 11- Связь со станцией связи СЕТУНЬ
    LOG_BRIDGETCP,              // 12- Шлюз с СПД
    LOG_LOGIC,                  // 13- Контроль несоответствия логических зависимостей
    LOG_BLANK                   // 14 -Пусто
};

enum LogTypes
{
    TYP_UNDEFINED = 0,
    LOG_ALARM = 1,              // 1 - Аварийное
    LOG_TECH,                   // 2 - Технологическое
    LOG_SYS,                    // 3 - Системное
    LOG_DIAG,                   // 4 - Диагностическое
    LOG_PROGRAMM,               // 5 - Программное
    LOG_FATAL_ERROR,            // 6 - Фатальная ошибка
    LOG_NOTIFY,                 // 7 - Уведомление
    LOG_TU,                     // 8 - Команда телеуправления
//------------------------------------------------------------------------------------------------------------------------
    LOG_SUPERALARM,             // 9 - Критическая ошибка. ВАЖНО: этого типа нет в SQL, поэтому вместо него пишется LOG_ALARM
//------------------------------------------------------------------------------------------------------------------------
};

extern ProgrammType modulType;     // тип приложения

extern bool IsMonitor  ();
extern bool IsArmTools ();
extern bool IsRss      ();
extern bool IsTablo    ();

// глобальные переменные
extern bool g_ShowStrlText  ;                               // отображение надписей на стрелках
extern bool g_ShowSvtfText  ;                               // отображение надписей на светофорах
extern bool g_ShowClosedSvtf;                               // отображение закрытых светофоров
extern bool g_ArchiveView;                                  // просмотр архива
extern bool g_bIgnoreServerTime;                            // глобальная переменная - флаг, запрещает синхронизацию часов и отрубает контроль
extern bool g_bNoSetServerTime ;                            // флаг ТОЛЬКО запрещает синхронизацию часов
extern time_t g_TmDtServer;                                 // серверное время
extern time_t g_DeltaTZ;                                    // разница времени удаленного сервера и настоящего АРМ ШН
extern int  g_RealStreamTsLength;                           // реальная длина однобитного блока данных ТС в актуальном потоке ТС; с учетом двухбитной передачи длина удваивается
extern bool g_QuickSearching;                               // флаг ускорееного сканирования входного потока
extern class Station * g_actualStation;                     // актуальная станция

const int MAX_DATA_LEN_FROM_MONITOR = 65535;

// глобальные загруженные ресурсы (значки и т.д.)
extern QPixmap  * g_green,
                * g_red,
                * g_yellow,
                * g_gray,
                * g_white,
                * g_cyan,

                * g_green_box,                              // МТС ок
                * g_green_box_tu,                           // МТУ ок
                * g_green_dark_box,                         // темно-зелееый
                * g_red_box,                                // МТС error
                * g_red_box_tu,                             // МТУ error
                * g_red_dark_box,                           // темно-красный
                * g_yellow_box,                             // желтый
                * g_yellow_dark_box,                        // коричневый
                * g_gray_box,
                * g_white_box,
                * g_green_box_blink;                        // индикаторы статуса

#endif // DEFINES_H
