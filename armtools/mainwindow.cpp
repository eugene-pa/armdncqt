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
#include "../common/inireader.h"
#include "../spr/streamts.h"

//QVector<ShapeSet *> sets;                                           // массив форм

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
    ui->setupUi(this);

    child = nullptr;

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

    reader = nullptr;

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

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
    ui->mainToolBar->insertWidget(ui->actionBlackbox, StationsCmb = new QComboBox);
    ui->mainToolBar->insertWidget(ui->actionPrev, dateEdit = new QDateEdit(QDate::currentDate()));
    dateEdit->setCalendarWidget(calendar = new QCalendarWidget());
    dateEdit->setCalendarPopup(true);
    ui->mainToolBar->insertWidget(ui->actionPrev, timeEdit = new QTimeEdit(QTime::currentTime()));

    ui->mainToolBar->addWidget(labelStep = new QLabel("  Шаг, мин: "));
    ui->mainToolBar->addWidget(stepValue = new QSpinBox(ui->mainToolBar));

    ui->mainToolBar->addWidget(labelTemp = new QLabel("     Темп,X:  1:1 "));
    ui->mainToolBar->addWidget(sliderTemp = new QSlider(Qt::Horizontal));
    ui->mainToolBar->addWidget(labelTemp = new QLabel(" 10:1"));
    sliderTemp->setTickPosition(QSlider::TicksAbove);
    sliderTemp->setFixedWidth(180);
    sliderTemp->setRange(1,10);
    sliderTemp->setValue(1);
    sliderTemp->setTickInterval(1);

    on_actionBlackbox_triggered();

    ui->mainToolBar->setBaseSize(800,36);

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

    // инициализация сетевых клиентов для подключения к серверу потока ТС
    clientTcp = new ClientTcp(server_ipport, &logger, false, "АРМ ШН");
    QObject::connect(clientTcp, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    clientTcp->start();
//    scale = 1;

    ui->toolBar2->addWidget(new QLabel("Масштаб:  ", ui->toolBar2));
    ui->toolBar2->addWidget(sliderScale = new QSlider(Qt::Horizontal, ui->toolBar2));
    sliderScale->setMinimum(-50);
    sliderScale->setMaximum(100);
    sliderScale->setValue(0);
    sliderScale->setTickPosition(QSlider::TicksAbove);
    sliderScale->setFixedWidth(180);

    connect(sliderScale, SIGNAL(valueChanged(int)), this, SLOT(scaleView()));

    ui->toolBar2->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::on_actionBlackbox_triggered()
{
    bool blackbox = ui->actionBlackbox->isChecked();
    dateEdit         ->setEnabled(blackbox);
    timeEdit         ->setEnabled(blackbox);
    ui->actionPrev   ->setEnabled(blackbox);
    ui->actionReverce->setEnabled(blackbox);
    ui->actionPlay   ->setEnabled(blackbox);
    ui->actionNext   ->setEnabled(blackbox);
    ui->action_Stop  ->setEnabled(blackbox);
    stepValue        ->setEnabled(blackbox);
    labelStep        ->setEnabled(blackbox);
    sliderTemp       ->setEnabled(blackbox);
}


// выбор станции в списке
void MainWindow::stationSelected(int index)
{
    Q_UNUSED(index)
    ShapeId * shapeId = (ShapeId *)StationsCmb->currentData().value<void *>();
    g_actualStation = shapeId->St();
    setCentralWidget(child = new ShapeChild(shapeId->Set()));

    emit changeStation(g_actualStation);
}

// обработка уведомлений ClientTcp потока ТС
// ВАЖНО: механизм слотов/сигналов позволяет изначально привязать разные экземпляры ClientTcp к разным слотам,
//        т.о.слоты должны быть типизированными под разные потоки и в теле слота не надо идентифицировать тип
// установлено соединение
void MainWindow::connected   (ClientTcp *client)
{
    hostStatus.setPixmap(*g_green);
    ui->statusBar->showMessage(QString("Соединение c хостом %1").arg(client->Name()), 60000);
}

// разорвано соединение
void MainWindow::disconnected(ClientTcp *client)
{
    hostStatus.setPixmap(*g_yellow);
    ui->statusBar->showMessage(QString("%1. Разрыв соединения c клиентом %2").arg(QTime::currentTime().toString()).arg(client->Name()), 60000);
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
    ui->statusBar->showMessage(QString("%1. Получены форматные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()), 10000);
    client->SendAck();                                      // квитирование

    // обработка данных
    DDataFromMonitor * pDtMntr = (DDataFromMonitor *)client->RawData();
    pDtMntr->Extract(client->RawLength());
}

// получены необрамленные данные - отдельный сигнал
void MainWindow::rawdataready(ClientTcp *client)
{
    ui->statusBar->showMessage(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()), 10000);
}



void MainWindow::on_action_TS_triggered()
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


void MainWindow::on_action_TU_triggered()
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

void MainWindow::on_action_OTU_triggered()
{

}

void MainWindow::on_action_Routes_triggered()
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

void MainWindow::on_action_KP_triggered()
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

void MainWindow::on_action_RC_triggered()
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

void MainWindow::on_action_STRL_triggered()
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

void MainWindow::on_action_SVTF_triggered()
{

}

void MainWindow::on_action_Stations_triggered()
{

}


void MainWindow::on_action_More_triggered()
{
    sliderScale->setValue(sliderScale->value() + 1);
    scaleView();                                    // масштабирование всего представления
}
void MainWindow::on_action_Less_triggered()
{
    sliderScale->setValue(sliderScale->value() - 1);
    scaleView();
}

void MainWindow::on_action_ZoomOff_triggered()
{
    sliderScale->setValue(0);
    scaleView();
}

// масштабирование всего представления
void MainWindow::scaleView()
{
    qreal scale = qPow(qreal(2), (sliderScale->value()) / qreal(50));
    QMatrix matrix;
    matrix.scale(scale, scale);
    child->setMatrix(matrix);
}

// вкл/откл панели масштабирования
void MainWindow::on_action_ToolBar2_triggered()
{
    ui->toolBar2->setVisible(ui->action_ToolBar2->isChecked());
}


// --------------------------------------------- Работа с архивом ----------------------------

// нажатие кнопки "Воспроизведение" (">")
void MainWindow::on_actionPlay_triggered()
{
    bPlay = ui->actionPlay->isChecked();
    if (bPlay)
    {
        reader = new ArhReader("c:/armdncqt/bd/temp/@_0.arh");
        bPlayBack = false;
        ui->actionNext->setEnabled(false);
        ui->actionPrev->setEnabled(false);
        ui->actionReverce->setEnabled(false);
        ui->action_Stop->setEnabled(true);
        idTimer = startTimer(500);
    }
}

// нажатие кнопки "СТОП" ("||")
void MainWindow::on_action_Stop_triggered()
{
    bPlay = bPlayBack = false;
    ui->actionNext->setEnabled(true);
    ui->actionPrev->setEnabled(true);
    ui->actionReverce->setEnabled(true);
    ui->action_Stop->setEnabled(false);
    killTimer(idTimer);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    qDebug() << "Таймер" << event->timerId();
    if (bPlay)
        readNext();
    else
    if (bPlayBack)
        readPrev();
}

// прочитать и отобразить след.запись в архиве
void MainWindow::readNext()
{
    qDebug() << "Следующая запись";
    if (reader != nullptr)
    {
        reader->Next();
        // обработать данные
        DDataFromMonitor * data = (DDataFromMonitor *)reader->Data();
        data->Extract(reader->Length());
    }
}

// прочитать и отобразить пред.запись в архиве
void MainWindow::readPrev()
{
    qDebug() << "Предыдущая запись";
}
