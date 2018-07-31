// файл с глобальными переменными группы проектов QtArmDnc
#include "defines.h"

AppTypes modulType;                                     // тип приложения

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

bool g_rqAck = true;                                        // ожидание квитанции от АРМ ДНЦ

class Station * g_actualStation;                            // актуальная станция
class ShapeId * g_actualForm;                               // актуальная схема

const char * AppNamesStr[] = {
                                "-",                        // 0
                                "Станция связи",            // 1
                                "Управление",               // 2
                                "Табло",                    // 3
                                "ТаблоМ",                   // 4
                                "График",                   // 5
                                "РСС МПК",                  // 6
                                "АРМ ШН",                   // 7
                                "Шлюз СПД",                 // 8
                                "Шлюз ГИД-УРАЛ",            // 9
                               };

const char * EventNames[] =   {
                                "-",                        // 0
                                "Аварийное",                // 1
                                "Технологическое",          // 2
                                "Системное",                // 3
                                "Диагностическое",          // 4
                                "Программное",              // 5
                                "Фатальная ошибка",         // 6
                                "Уведомление",              // 7
                                "Телеуправление",           // 8
                                "ОТУ",                      // 9
                               };


QPixmap * g_green,
        * g_red,
        * g_yellow,
        * g_gray,
        * g_white,
        * g_cyan,

        * g_green_box,
        * g_green_box_tu,                                   // МТУ ок
        * g_green_dark_box,
        * g_red_box,
        * g_red_box_tu,                                     // МТУ error
        * g_red_dark_box,
        * g_yellow_box,
        * g_yellow_dark_box,
        * g_gray_box,
        * g_white_box,
        * g_green_box_blink,                                // индикаторы статуса
        * g_strl_minus,                                     // -
        * g_strl_plus;                                      // +

//  COMMTIMEOUTS tm = { 10,0,400,1,500};


bool IsMonitor  () { return modulType==APP_MONITOR  ; }
bool IsArmTools () { return modulType==APP_ARMUTILS ; }
bool IsRss      () { return modulType==APP_MDMAGENT ; }
bool IsTablo    () { return modulType==APP_TABLO    ; }

// имя приложения по коду
const char * GetAppNameById(int id)
{
    return (id>=0 || id <= APP_GIDURAL) ? AppNamesStr[id] : AppNamesStr[0];
}

// тип события по коду
const char * GetEventTypeNameById(int id)
{
    return (id>=0 || id <= APP_GIDURAL) ? EventNames[id] : EventNames[0];
}


// 1. Побайтовый алгоритм вычисления CRC
//    (Р.Л.Хаммел,"Послед.передача данных", Стр.49. М.,Мир, 1996)
const WORD CRC_POLY = 0x1021;
//#define BYTE unsigned char
//#define WORD unsigned short
void addCRC (QByteArray& data)
{
    WORD j,w,crc = 0;
    for (int i=0; i<data.length(); i++)
    {
        w = (WORD)((BYTE)data[i])<<8;
        crc ^= w;
        for (j=0; j<8; j++)
            if (crc & 0x8000)
                crc = (crc<<1) ^ CRC_POLY;
            else
                crc = crc << 1;
    }
    data.append(crc & 0x00ff);
    data.append((crc >> 8) & 0x00ff);
}
// перегруженная функция вычисления CRC
void addCRC (void *ptr, int length)
{
    *((WORD *)&((BYTE*)ptr)[length]) = GetCRC (ptr, length);
}

// ВЫНЕСТИ В ОБЩИЙ ФАЙЛ
// 1. Побайтовый алгоритм вычисления CRC
//    (Р.Л.Хаммел,"Послед.передача данных", Стр.49. М.,Мир, 1996)
WORD GetCRC (void *ptr,int Len)
{
    BYTE * buf = (BYTE *)ptr;
    WORD j,w,Crc = 0;
    while (Len--)
    {
        w = (WORD)(*buf++)<<8;
        Crc ^= w;
        for (j=0; j<8; j++)
            if (Crc & 0x8000)
                Crc = (Crc<<1) ^ CRC_POLY;
            else
                Crc = Crc << 1;
    }
    return Crc;
}

// загрузка типовых ресурсов
bool loadResources(QString dir)
{
    g_green             = new QPixmap(dir + "icon_grn.ico");
    g_red               = new QPixmap(dir + "icon_red.ico");
    g_yellow            = new QPixmap(dir + "icon_yel.ico");
    g_gray              = new QPixmap(dir + "icon_gry.ico");
    g_white             = new QPixmap(dir + "icon_wht.ico");
    g_cyan              = new QPixmap(dir + "icon_cyn.ico");

    g_green_box_blink   = new QPixmap(dir + "box_grn_blink.ico");
    g_green_box         = new QPixmap(dir + "box_grn.ico");
    g_green_box_tu      = new QPixmap(dir + "box_grn_tu.ico");           // МТУ ок
    g_green_dark_box    = new QPixmap(dir + "box_grn_dark.ico");
    g_red_box           = new QPixmap(dir + "box_red.ico");
    g_red_box_tu        = new QPixmap(dir + "box_red_tu.ico");           // МТУ error
    g_red_dark_box      = new QPixmap(dir + "box_red_dark.ico");
    g_yellow_box        = new QPixmap(dir + "box_yel.ico");
    g_yellow_dark_box   = new QPixmap(dir + "box_yel_dark.ico");
    g_gray_box          = new QPixmap(dir + "box_gry.ico");
    g_white_box         = new QPixmap(dir + "box_wht.ico");

    g_strl_minus        = new QPixmap(dir + "strl_minus.ico");           // -
    g_strl_plus         = new QPixmap(dir + "strl_plus.ico");            // +

    return !(g_green          ->isNull() ||
            g_red            ->isNull() ||
            g_yellow         ->isNull() ||
            g_gray           ->isNull() ||
            g_white          ->isNull() ||
            g_cyan           ->isNull() ||
            g_green_box_blink->isNull() ||
            g_green_box      ->isNull() ||
            g_green_box_tu   ->isNull() ||
            g_green_dark_box ->isNull() ||
            g_red_box        ->isNull() ||
            g_red_box_tu     ->isNull() ||
            g_red_dark_box   ->isNull() ||
            g_yellow_box     ->isNull() ||
            g_yellow_dark_box->isNull() ||
            g_gray_box       ->isNull() ||
            g_white_box      ->isNull() ||
            g_strl_minus     ->isNull() ||
            g_strl_plus      ->isNull());
}

// формирование полного пути из относительного
// если путь относительный - меняем и возвращаем true
// если путь полный - ничего не менять и вернуть false
bool makeFullPath(QString base, QString& path)
{
    QFileInfo fi(path);
    if (fi.isRelative())
    {
        path = base + "/" + path;
        return true;
    }
    return false;
}

// рекомендованные реализации для преобразования QString <-> wstring
std::wstring qToStdWString(const QString &str)
{
#ifdef _MSC_VER
 return std::wstring((const wchar_t*)str.utf16());          // MSVC
#else
 return str.toStdWString();                                 // GCC
#endif
}

QString stdWToQString(const std::wstring &str)
{
#ifdef _MSC_VER
 return QString::fromUtf16((const ushort*)str.c_str());     // MSVC
#else
 return QString::fromStdWString(str);                       // GCC
#endif
}

