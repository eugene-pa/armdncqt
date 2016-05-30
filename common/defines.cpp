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
