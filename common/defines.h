﻿#ifndef DEFINES_H
#define DEFINES_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QGraphicsItem>
#include <QColor>
#include "QCloseEvent"
#include <QFont>
#include <QBrush>
#include <QPen>

#include <time.h>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QBitArray>
#include <QVariant>
#include "QDateTime"
#include "QRegularExpression"
#include <QByteArray>

#ifdef Q_OS_WIN
#include <io.h>
#include <fcntl.h>
#endif // #ifdef Q_OS_WIN

//#include <QWidget>
//#include <QGraphicsView>

#define nullptr 0

// определения, облегчающие перенос кода с MSVC&MFC в QT
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
//typedef unsigned int    DWORD;
typedef unsigned int    UINT;
#ifndef Q_OS_WIN
typedef const char *    LPCTSTR;
#endif

#define SIGNATURE    0xAA55                                 // сигнатура пакета
#define SIGNATUREZIP 0x55AA                                 // сигнатура сжатого пакета
#define DUBL 2
#define MAX_TRAINS_ON_PER 5
#define TRACE(param) qDebug() << param
// #define TRACE(x) qDebug() << QString::fromWCharArray(x)  - для явно определенного юникода

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

#define FORMAT_DATETIME "dd.MM.yy hh:mm:ss"
#define FORMAT_TIME     "hh:mm:ss"
#define FORMAT_DATE     "dd.MM.yy"

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
extern bool loadResources(QString dir);

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
extern bool g_QuickSearching;                               // флаг ускоренного сканирования входного потока

extern bool g_rqAck;                                        // требовать явное подтверждение ввода команд нажатием кнопки

extern class Station * g_actualStation;                     // актуальная станция
extern class ShapeId * g_actualForm;                        // актуальная схема

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
                * g_green_box_blink,                        // индикаторы статуса

                * g_strl_minus,                             // -
                * g_strl_plus;                              // +

#ifndef _WINBASE_
typedef struct _COMMTIMEOUTS {
    UINT ReadIntervalTimeout;          /* Maximum time between read chars. */
    UINT ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
    UINT ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
    UINT WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
    UINT WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;
#endif // #ifndef _COMMTIMEOUTS

extern void addCRC (QByteArray& data);
extern bool makeFullPath(QString base, QString& path);

#define varfromptr(x) qVariantFromValue((void *)x)
#define ptrfromvar(x) x.value<void*>()

std::wstring qToStdWString(const QString &str);
QString stdWToQString(const std::wstring &str);

#endif // DEFINES_H
