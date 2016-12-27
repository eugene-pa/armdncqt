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

QString images(":/icons/images/");                          // путь к образам status/images

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

    g_actualStation = nullptr;
    g_actualForm    = nullptr;

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

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

    // загрузка пользовательской графики (можно вынести в глоб.функцию)
    loadResources();

    // значок приложения
    QIcon i(QPixmap(images + "monitor.png"));
    setWindowIcon (QIcon(QPixmap(images + "monitor2.png")));

    // создаем меню ПОМОЩЬ справа (используем setCornerWidget для отдельного QMenuBar)
    QMenuBar *bar = new QMenuBar(ui->menuBar);
    QMenu *menu = new QMenu("Помощь", bar);
    bar->addMenu(menu);

    QAction *action1 = new QAction("О программе", bar);
    menu->addAction(action1);
    connect(action1, SIGNAL(triggered()), this, SLOT(on_action_About_triggered()));

    ui->menuBar->setCornerWidget(bar);

    // добавляем тулбар ввода ТУ
    fontToolbar = new QFont("Segoe UI",12);
    extBar = new QToolBar(this);
    extBar->setFont(*fontToolbar);
    extBar->setAllowedAreas(Qt::TopToolBarArea);
    extBar->addWidget(new QLabel("Команда ТУ: [F9]",extBar));
    extBar->addWidget(new QLineEdit("", extBar));
    extBar->addAction(ui->action_Enter);
    extBar->addAction(ui->action_mnvr_ON);
    extBar->addAction(ui->action_var_ON);
    extBar->addAction(ui->action_nprv_ON);
    extBar->addAction(ui->action_checktu_OFF);
    extBar->setIconSize(QSize(32,32));

    addToolBarBreak();
    addToolBar(extBar);

    // загрузка НСИ
    KrugInfo * krug = nullptr;
    Esr::ReadBd(esrdbbname, logger);                        // ЕСР
    Station::ReadBd(dbname, krug, logger);                  // станции
    Peregon::ReadBd(dbname, krug, logger);                  // перегоны
    IdentityType::ReadBd (extDb, logger);                   // описание свойств и методов объектов (таблица Properties)
    Ts::ReadBd (dbname, krug, logger);                      // ТС
    Tu::ReadBd (dbname, krug, logger);                      // ТУ
    Otu::ReadBd (dbname, krug, logger);
    Rc::ReadRelations(dbname, logger);                      // связи РЦ
    Route::ReadBd(dbname, krug, logger);                    // маршруты
    Pereezd::ReadBd (dbname, krug, logger);                 // переезды

    DShape::InitInstruments(extDb, logger);                 // инициализация графических инструментов
    ShapeSet::ReadShapes(formDir, &logger);                 // чтение форм

    stationSelected(Station::GetById(1)->formList[0]);

    // размещаем кнопки выбора станций в QVBoxLayout
    // проблема: расположение кнопок в старой версии задавалось либо номером кнопки, либо перечнем станций в таблице Krugs[Контроль станций]
    ui->frame_st->setLayout(new  QVBoxLayout(ui->frame_st));
    for (Station * st : Station::StationsOrg)
    {
        for (ShapeId * id : st->formList)
        {
            QPushButton * button = new QPushButton(id->Name());
            id->setButton(button);
            button->setCheckable(true);
            button->setProperty("shapeID", varfromptr(id));
            ui->frame_st->layout()->addWidget(button);

            connect(button, SIGNAL(released()), this, SLOT (btnStation()));
        }
    }
}

void MainWindow::btnStation()
{
    if (g_actualForm && g_actualForm->getButton())
        g_actualForm->getButton()->setChecked(false);
    QPushButton * btn = (QPushButton *)sender();
    if (!btn->isChecked())
        btn->setChecked(true);
    else
    {
        g_actualForm = (ShapeId *)ptrfromvar(btn->property("shapeID"));
        //g_actualStation = g_actualForm->St();
        qDebug() << g_actualForm->Name();
        stationSelected(g_actualForm);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_About_triggered()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );
    QMessageBox::about(this, "О программе", QString("ДЦ ЮГ. УПРАВЛЕНИЕ\n%1\n\nФайл: %2.\nДата сборки: %3\n© ООО НПЦ Промавтоматика, 2016").arg(version).arg(info.filePath()).arg(info.lastModified().toString(FORMAT_DATETIME)));
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

void MainWindow::stationSelected(ShapeId * shapeId)
{
    if (shapeId->St() != nullptr)
        g_actualStation = shapeId->St();
    if (child != nullptr)
        delete child;

    // 2016.12.12. Вопрос: правильно ли это - каждый раз создавать класс ShapeChild заново? Кто удаляет текущий класс ?

    child = new ShapeChild(shapeId->Set());
    //ui->centralWidget->layout()->addWidget(ui->frame_st);
    ui->centralWidget->layout()->addWidget(child);
    //child->setMouseTracking(tooltip);

//    scaleView();
/*
    child->horizontalScrollBar()->setValue(0);
    child->verticalScrollBar()->setValue(0);
    child->centerOn(0,0);
*/
}

// масштабирование всего представления
//void MainWindow::scaleView()
//{
//    qreal scale = qPow(qreal(2), (sliderScale->value()) / qreal(50));
//    QTransform transform;
//    transform.scale(scale, scale);
//    child->setTransform(transform);
//}
