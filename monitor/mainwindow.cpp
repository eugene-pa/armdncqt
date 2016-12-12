#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"

#include "../forms/dlgrcinfo.h"
#include "../forms/dlgstrlinfo.h"
#include "../forms/dlgsvtfinfo.h"
#include "../forms/dlgstationsinfo.h"
#include "../forms/dlgtsinfo.h"
#include "../forms/dlgtuinfo.h"
#include "../forms/dlgotu.h"
#include "../forms/dlgkpinfo.h"
#include "../forms/dlgroutes.h"
#include "../forms/dlgtrains.h"
#include "../forms/dlgperegoninfo.h"
#include "../forms/dlgpereezd.h"
#include "../common/inireader.h"
#include "../spr/streamts.h"
#include "../spr/train.h"
#include "../shapes/shapetrain.h"

QString version = "1.0.1.10";                               // версия приложения
QString title = "ДЦ ЮГ. УПРАВЛЕНИЕ. ";

QString images(":/images/icons/");                          // путь к образам status/images

#ifdef Q_OS_WIN
    QString path = "C:/armdncqt/";
    QString compressor = "c:/armdncqt/bin/zip.exe";         // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "c:/armdncqt/bin/unzip.exe";     // утилита для распаковки архивов
    QString editor = "notepad.exe";                         // блокнот
#endif
#ifdef Q_OS_MAC
    QString path = "/Users/evgenyshmelev/armdncqt/";
    QString compressor = "zip";                             // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "unzip";                         // утилита для распаковки архивов
    QString editor = "TextEdit";                             // блокнот

#endif
#ifdef Q_OS_LINUX
    QString path = "/home/dc/armdncqt/";
    QString compressor = "zip";                             // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "unzip";                         // утилита для распаковки архивов
    QString editor = "gedit";                               // блокнот
#endif
    Logger logger(path + "log/monitor.log", true, true);

    QString dbname      = path + "bd/arm.db";
    QString esrdbbname  =        "bd/arm.db";
    QString extDb       = path + "bd/armext.db";
    QString pathTemp    = path + "bd/temp/";
    QString pathSave    = path + "bd/save/";
    QString formDir     = path + "pictures/";
    QString iniFile     =        "monitor.ini";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->setIconSize(QSize(64,40));

    child = nullptr;

#ifdef Q_OS_WIN
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("CP866"));
#endif

    modulType=APP_MONITOR;                                  // тип приложения

    dlgTs = nullptr;                                        // состояние ТС
    dlgTu = nullptr;                                        // список ТУ
    dlgOtu = nullptr;                                       // список ОТУ
    dlgRc = nullptr;                                        // состояние РЦ
    dlgStrl = nullptr;                                      // состояние стрелок
    dlgSvtf = nullptr;                                      // состояние светофоров
    dlgKp = nullptr;                                        // диалог КП
    dlgRoutes = nullptr;                                    // диалог маршрутов
    dlgTrains = nullptr;                                    // поезда
    dlgStations = nullptr;                                  // станции
    dlgPeregons = nullptr;                                  // перегоны
    dlgPereezd = nullptr;                                   // переезды

    // если задан конфигурационный файл, читаем настройки и подстраиваем пути
    IniReader rdr(iniFile);
    if (rdr.GetText("WORKINDIRECTORY", path))               // рабочая папка
    {
        dbname = path + "bd/arm.db";
        extDb  = path + "bd/armext.db";
        pathTemp=path + "bd/temp/";
        pathSave=path + "bd/save/";
        formDir =path + "pictures/";

        logger.ChangeActualFile(path + "log/monitor.log");
    }

    QString tmp;

    // опция ESRDBNAME
    if (rdr.GetText("ESRDBNAME", tmp))
        esrdbbname = QFileInfo(tmp).isAbsolute() ? tmp : path + tmp;
    else
        esrdbbname = path + esrdbbname;

    // опция TITLE
    if (rdr.GetText("TITLE", tmp))
        title += tmp;

    this->setWindowTitle(title);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{

}

void MainWindow::on_action_2_triggered()
{

}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
}

void MainWindow::loadResources()
{
    g_green             = new QPixmap(images + "icon_grn.ico");
    g_red               = new QPixmap(images + "icon_red.ico");
    g_yellow            = new QPixmap(images + "icon_yel.ico");
    g_gray              = new QPixmap(images + "icon_gry.ico");
    g_white             = new QPixmap(images + "icon_wht.ico");
    g_cyan              = new QPixmap(images + "icon_cyn.ico");

    g_green_box_blink   = new QPixmap(images + "box_grn_blink.ico");
    g_green_box         = new QPixmap(images + "box_grn.ico");
    g_green_box_tu      = new QPixmap(images + "box_grn_tu.ico");           // МТУ ок
    g_green_dark_box    = new QPixmap(images + "box_grn_dark.ico");
    g_red_box           = new QPixmap(images + "box_red.ico");
    g_red_box_tu        = new QPixmap(images + "box_red_tu.ico");           // МТУ error
    g_red_dark_box      = new QPixmap(images + "box_red_dark.ico");
    g_yellow_box        = new QPixmap(images + "box_yel.ico");
    g_yellow_dark_box   = new QPixmap(images + "box_yel_dark.ico");
    g_gray_box          = new QPixmap(images + "box_gry.ico");
    g_white_box         = new QPixmap(images + "box_wht.ico");

    g_strl_minus        = new QPixmap(images + "strl_minus.ico");           // -
    g_strl_plus         = new QPixmap(images + "strl_plus.ico");            // +

    tooltip = true;
}
