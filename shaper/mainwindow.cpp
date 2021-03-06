#include <QTextCodec>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"
#include "../forms/dlgrcinfo.h"
#include "../forms/dlgstrlinfo.h"
#include "../forms/dlgstationsinfo.h"
#include "../forms/dlgtsinfo.h"
#include "../forms/dlgtuinfo.h"
#include "../forms/dlgkpinfo.h"
#include "../forms/dlgroutes.h"
#include "../forms/dlgtrains.h"
#include "../spr/krug.h"
#include "../common/inireader.h"


//Logger logger("Log/shaper.txt", true, true);
QVector<ShapeSet *> sets;                                           // массив форм

//QString server_ipport = "192.168.0.101:1010";                       // подключение к потоку ТС из настроечного файла
QString server_ipport = "192.168.0.100:1013";                       // подключение к потоку ТС из настроечного файла
QString baseDir;

#ifdef Q_OS_WIN
    Logger logger("Log/shaper.txt", true, true);
    QString dbname("C:/armdncqt/bd/arm.db");
    QString extDb ("C:/armdncqt/bd/armext.db");
    QString form  ("C:/armdncqt/pictures/Назаровский.shp");         // Табло1
    QString formDir("C:/armdncqt/pictures/");
    QString images(":/status/images/");                                   // путь к образам
    QString iniFile = "c:/armdncqt/shaper/shaper.ini";
#endif
#ifdef Q_OS_MAC
    Logger logger("/Users/evgenyshmelev/armdncqt/Log/shaper.txt", true, true);
    QString dbname("/Users/evgenyshmelev/armdncqt/bd/arm.db");
    QString extDb ("/Users/evgenyshmelev/armdncqt/bd/armext.db");
    QString form  ("/Users/evgenyshmelev/armdncqt/Pictures/Назаровский.shp");
    QString formDir("/Users/evgenyshmelev/armdncqt/Pictures/");
    QString images("/Users/evgenyshmelev/armdncqt/images/");       // путь к образам
    QString iniFile = "/Users/evgenyshmelev/armdncqt/shaper/shaper.ini";
#endif
#ifdef Q_OS_LINUX
    Logger logger("/home/eugene/QTProjects/armdncqt/Log/shaper.txt", true, true);
    QString dbname("/home/eugene/QTProjects/armdncqt/bd/arm.db");
    QString extDb ("/home/eugene/QTProjects/armdncqt/bd/armext.db");
    QString form  ("/home/eugene/QTProjects/armdncqt/pictures/Назаровский.shp");
    QString formDir  ("/home/eugene/QTProjects/armdncqt/pictures/");
    QString images("../images/");                                   // путь к образам
    QString iniFile = "/home/eugene/QTProjects/armdncqt/shaper/shaper.ini";
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // устанавливаем кодировку для отладочного окна вывода
#ifdef Q_OS_WIN
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("CP866"));
#endif

    dlgTs = nullptr;                                                    // состояние ТС
    dlgTu = nullptr;                                                    // состояние ТУ
    dlgRc = nullptr;                                                    // состояние РЦ
    dlgStrl = nullptr;
    dlgKp = nullptr;                                                    // диалог КП
    dlgRoutes = nullptr;                                                // диалог маршрутов
    dlgTrains = nullptr;                                                // поезда

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

    IniReader rdr(iniFile);
    QString s;
    rdr.GetText("BASEDIR", baseDir);
    rdr.GetText("DBPATH", s);


    bool b;
    rdr.GetBool("MONITORING", b);

    int n;
    rdr.GetInt("ALLRAS", n);


    ui->setupUi(this);

    // подготовка контейнера MDI
    mdiArea = new QMdiArea;                                             // создаем виджет MDI
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);       // скролбары
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    // загрузка пользовательской графики (можно вынести в глоб.функцию)
    loadResources();

    // добавляем в статус бар поля "IP_ПОРТ" и индикатор соединения
    ui->statusBar->addPermanentWidget(new QLabel(server_ipport));   //
    hostStatus.setPixmap(*g_yellow);
    ui->statusBar->addPermanentWidget(&hostStatus);

    // загрузка НСИ
    KrugInfo * krug = nullptr;
    Esr::ReadBd(dbname, logger);                            // ЕСР
    Station::ReadBd(dbname, krug, logger);                  // станции
    IdentityType::ReadBd (extDb, logger);                   // описание свойств и методов объектов (таблица Properties)
    Ts::ReadBd (dbname, krug, logger);                      // ТС
    Tu::ReadBd (dbname, krug, logger);                      // ТУ
    Rc::ReadRelations(dbname, logger);                      // связи РЦ
    Route::ReadBd(dbname, krug, logger);                    // маршруты   
    DShape::InitInstruments(extDb, logger);                 // инициализация графических инструментов

    ShapeSet::ReadShapes(formDir, &logger);                 // чтение форм

    // создаем комбо бокс выбора станций, заполняем и привязываем сигнал currentIndexChanged к слоту-обработчику
    ui->mainToolBar->insertWidget(ui->actionBlackBox, StationsCmb = new QComboBox);
    ui->mainToolBar->insertWidget(ui->actionNewForm, dateEdit = new QDateEdit(QDate::currentDate()));
    dateEdit->setCalendarWidget(calendar = new QCalendarWidget());
    dateEdit->setCalendarPopup(true);
    ui->mainToolBar->insertWidget(ui->actionNewForm, timeEdit = new QTimeEdit(QTime::currentTime()));
    dateEdit->setEnabled(false);
    timeEdit->setEnabled(false);


    calendar->hide();
    timeEdit->setDisplayFormat("hh:mm:ss");


    foreach (Station * st, Station::Stations.values())
    {
        foreach (ShapeId * p, st->formList)
          {
              StationsCmb->addItem(p->Name(), qVariantFromValue((void *) p));
          }
    }
    QObject::connect(StationsCmb, SIGNAL(currentIndexChanged(int)), SLOT(stationSelected(int)));
    StationsCmb->model()->sort(0);
    StationsCmb->setCurrentIndex(0);
    g_actualStation = ((ShapeId *)StationsCmb->currentData().value<void *>())->St();


    // инициализация сетевых клиентов для подключения к серверу потока ТС
    clientTcp = new ClientTcp(server_ipport, &logger, false, "АРМ ШН");
    QObject::connect(clientTcp, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    clientTcp->start();
}

MainWindow::~MainWindow()
{
    logger.log ("Завершение приложения");
    qDeleteAll(sets.begin(), sets.end());                           // удаляем примитивы
    delete ui;
}

void Log (QString msg)
{
    logger.log(msg);
}


// новая форма
void MainWindow::on_actionNewForm_triggered()
{
    ShapeId * set = (ShapeId *)StationsCmb->currentData().value<void*>();
    ShapeChild * child = new ShapeChild(set->Set());
    mdiArea->addSubWindow(child);
    child->show();
    //ShapeChild
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Варилка.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Васюринская.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Гетмановская.shp"));
}



// обработка уведомлений ClientTcp потока ТС
// ВАЖНО: механизм слотов/сигналов позволяет изначально привязать разные экземпляры ClientTcp к разным слотам,
//        т.о.слоты должны быть типизированными под разные потоки и в теле слота не надо идентифицировать тип
// установлено соединение
void MainWindow::connected   (ClientTcp *client)
{
    hostStatus.setPixmap(*g_green);
    ui->statusBar->showMessage(QString("Соединение c хостом %1").arg(client->name()), 60000);
}

// разорвано соединение
void MainWindow::disconnected(ClientTcp *client)
{
    hostStatus.setPixmap(*g_yellow);
    ui->statusBar->showMessage(QString("%1. Разрыв соединения c клиентом %2").arg(QTime::currentTime().toString()).arg(client->name()), 60000);
}

// ошибка сокета
void MainWindow::error (ClientTcp *client)
{
    Q_UNUSED(client)
    hostStatus.setPixmap(*g_red);
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void MainWindow::dataready   (ClientTcp * client)
{
    ui->statusBar->showMessage(QString("%1. Получены форматные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->rawLength()), 10000);
    client->sendAck();                                      // квитирование

    // обработка данных
    DDataFromMonitor * pDtMntr = (DDataFromMonitor *)client->rawData();
    pDtMntr->Extract(client->rawLength());
}

// получены необрамленные данные - отдельный сигнал
void MainWindow::rawdataready(ClientTcp *client)
{
    ui->statusBar->showMessage(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->rawLength()), 10000);
}


// выбор станции в списке
void MainWindow::stationSelected(int index)
{
    Q_UNUSED(index)
    g_actualStation = ((ShapeId *)StationsCmb->currentData().value<void *>())->St();
    emit changeStation(g_actualStation);
}


// ----------------------------------------------------------------------------------------
// вызов диалогов табличной информации об объектах: информация по РЦ, стрелкам, сигналам
// ----------------------------------------------------------------------------------------

// обработчик меню информация по синалам ТС
void MainWindow::on_action_TsInfo_triggered()
{
    if (dlgTs==nullptr)
    {
        dlgTs = new DlgTsInfo(this, g_actualStation);
        dlgTs->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgTs, SLOT(changeStation(Station*)));
    }
    else
        dlgTs->setVisible(!dlgTs->isVisible());
}

// обработчик меню информация по синалам ТУ
void MainWindow::on_action_TuInfo_triggered()
{
    if (dlgTu==nullptr)
    {
        dlgTu = new DlgTuInfo(this, g_actualStation);
        dlgTu->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgTu, SLOT(changeStation(Station*)));
    }
    else
        dlgTu->setVisible(!dlgTu->isVisible());
}

// обработчик меню информация по РЦ
void MainWindow::on_action_RcInfo_triggered()
{
    if (dlgRc==nullptr)
    {
        dlgRc = new DlgRcInfo(g_actualStation, this);
        dlgRc->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgRc, SLOT(changeStation(Station*)));
    }
    else
        dlgRc->setVisible(!dlgRc->isVisible());
}

// обработчик меню информация по КП
void MainWindow::on_action_KPinfo_triggered()
{
    if (dlgKp==nullptr)
    {
        dlgKp = new DlgKPinfo(g_actualStation, this);
        dlgKp->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgKp, SLOT(changeStation(Station*)));
    }
    else
        dlgKp->setVisible(!dlgKp->isVisible());
}


// обработчик меню информация по маршрутам
void MainWindow::on_action_RouteInfo_triggered()
{
    if (dlgRoutes==nullptr)
    {
        dlgRoutes = new DlgRoutes(g_actualStation, this);
        dlgRoutes->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgRoutes, SLOT(changeStation(Station*)));
    }
    else
        dlgRoutes->setVisible(!dlgRoutes->isVisible());
}

// обработчик меню информация по стрелкам
void MainWindow::on_action_StrlInfo_triggered()
{
    if (dlgStrl == nullptr)
    {
        dlgStrl = new DlgStrlInfo(g_actualStation, this);
        dlgStrl->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgStrl, SLOT(changeStation(Station*)));
    }
    else
        dlgStrl->setVisible(!dlgStrl->isVisible());
}

// обработчик меню информация по поездам
void MainWindow::on_action_trainsInfo_triggered()
{
    if (dlgTrains == nullptr)
    {
        dlgTrains = new DlgTrains(this);
        dlgTrains->show();
    }
    else
        dlgTrains->setVisible(!dlgTrains->isVisible());
}

// обработчик меню информация по светофорам
void MainWindow::on_action_SvtfInfo_triggered()
{

}

// обработчик меню информация по станциям
void MainWindow::on_action_stationsInfo_triggered()
{
    DlgStationsInfo * dlg = new DlgStationsInfo(this);
    dlg->show();
}


// обработчик меню информация по перегонам
void MainWindow::on_action_prgInfo_triggered()
{

}


// обработчик меню информация по синалам ОТУ
void MainWindow::on_action_OtuInfo_triggered()
{
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

}

void MainWindow::on_actionBlackBox_triggered()
{
    dateEdit->setEnabled(ui->actionBlackBox->isChecked());
    timeEdit->setEnabled(ui->actionBlackBox->isChecked());
}
