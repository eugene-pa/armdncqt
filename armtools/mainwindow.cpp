#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"

#include "../forms/dlgrcinfo.h"
#include "../forms/dlgstrlinfo.h"
#include "../forms/dlgsvtfinfo.h"
#include "../forms/dlgstationsinfo.h"
#include "../forms/dlgtsinfo.h"
#include "../forms/dlgtuinfo.h"
#include "../forms/dlgkpinfo.h"
#include "../forms/dlgroutes.h"
#include "../forms/dlgtrains.h"
#include "../common/inireader.h"
#include "../spr/streamts.h"

//QVector<ShapeSet *> sets;                                 // массив форм

//QString server_ipport = "192.168.0.101:1020";             // подключение к потоку ТС из настроечного файла
QString server_ipport = "192.168.0.100:1013";               // подключение к потоку ТС из настроечного файла
QString baseDir;
QString version = "1.0.1.10";                               // версия приложения

//bool MainWindow::blackBoxMode;                              // включен режима просмотра архива
bool blackBoxMode;                                          // включен режима просмотра архива

#ifdef Q_OS_WIN
    QString path = "C:/armdncqt/";
    QString images(":/status/images/");                     // путь к образам
    QString imagesEx(":/images/images/");                   //
    QString compressor = "c:/armdncqt/bin/zip.exe";         // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "c:/armdncqt/bin/unzip.exe";     // утилита для распаковки архивов
    QString editor = "notepad.exe";     // блокнот
#endif
#ifdef Q_OS_MAC
    QString path = "/Users/evgenyshmelev/armdncqt/";
    QString images(":/status/images/");                       // путь к образам
    QString imagesEx(":/images/images/");                     // путь к образам
    QString compressor = "zip";                             // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "unzip";                         // утилита для распаковки архивов
    QString editor = "TextEdit";                             // блокнот

#endif
#ifdef Q_OS_LINUX
    QString path = "/home/dc/armdncqt/";
    QString images  (":/status/images/");                   // путь к образам   было "../images/"
    QString imagesEx(":/images/images/");                   // путь к образам   было "../images/"
    QString compressor = "zip";                             // утилита для сжатия файлов в архивы (zip АРХИВ ШАБЛОН_ИЛИ_СПИСОК)
    QString decompressor = "unzip";                         // утилита для распаковки архивов
    QString editor = "gedit";                               // блокнот
#endif
    Logger logger(path + "Log/armtools.log", true, true);
    QString dbname = path + "bd/arm.db";
    QString extDb  = path + "bd/armext.db";
    QString pathTemp=path + "bd/temp/";
    QString pathSave=path + "bd/save/";
    QString form    =path + "pictures/Назаровский.shp";
    QString formDir =path + "pictures/";
    QString iniFile =path + "armtools/armtools.ini";

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

    modulType=APP_ARMUTILS;                                             // тип приложения

    dlgTs = nullptr;                                                    // состояние ТС
    dlgTu = nullptr;                                                    // состояние ТУ
    dlgRc = nullptr;                                                    // состояние РЦ
    dlgStrl = nullptr;                                                  // состояние стрелок
    dlgSvtf = nullptr;                                                  // состояние светофоров
    dlgKp = nullptr;                                                    // диалог КП
    dlgRoutes = nullptr;                                                // диалог маршрутов
    dlgTrains = nullptr;                                                // поезда
    dlgStations = nullptr;                                              // станции

    reader = nullptr;

    // если задан конфигурационный файл, читаем настройки и подстраиваем пути
    // iniFile = "armtoola.ini";                           // так будем брать настройки из тек.каталога, если ini-файл не задан в параметрах
    IniReader rdr(iniFile);
    if (rdr.GetText("WORKINDIRECTORY", path))
    {
        dbname = path + "bd/arm.db";
        extDb  = path + "bd/armext.db";
        pathTemp=path + "bd/temp/";
        pathSave=path + "bd/save/";
        form    =path + "pictures/Назаровский.shp";
        formDir =path + "pictures/";

        logger.ChangeActualFile(path + "Log/armtools.log");
    }


//    QByteArray src(10,0x55);
//    QByteArray dst = qCompress(src);
//    dst[3] = 0x0;
//    QByteArray src2 = qUncompress(dst);

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

    // загрузка пользовательской графики (можно вынести в глоб.функцию)
    loadResources();
    // значок приложения
    setWindowIcon (QIcon(QPixmap(imagesEx + "Config-Tools.png")));

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

    // динамическое формирование элементов тулбара
    // создаем комбо бокс выбора станций, заполняем и привязываем сигнал currentIndexChanged к слоту-обработчику
    ui->mainToolBar->insertWidget(ui->actionBlackbox, StationsCmb = new QComboBox);                 // "Черный ящик"
    ui->mainToolBar->insertWidget(ui->actionPrev, dateEdit = new QDateEdit(QDate::currentDate()));  // Дата
    dateEdit->setCalendarWidget(calendar = new QCalendarWidget());                                  // Календарь
    dateEdit->setCalendarPopup(true);
    ui->mainToolBar->insertWidget(ui->actionPrev,new QLabel(" "));                                  // Пробнл
    ui->mainToolBar->insertWidget(ui->actionPrev, timeEdit = new QTimeEdit(QTime::currentTime()));  // Время



    ui->mainToolBar->addWidget(labelStep = new QLabel("  Шаг, мин: "));                             // Шаг по минутам
    ui->mainToolBar->addWidget(stepValue = new QSpinBox(ui->mainToolBar));

    ui->mainToolBar->addWidget(labelTemp = new QLabel("     Темп,X:  1:1 "));                       // Темп
    ui->mainToolBar->addWidget(sliderTemp = new QSlider(Qt::Horizontal));                           // Задатчик-слайдер
    ui->mainToolBar->addWidget(labelTemp2 = new QLabel(" 10:1"));
    sliderTemp->setTickPosition(QSlider::TicksAbove);
    sliderTemp->setFixedWidth(180);
    sliderTemp->setRange(1,10);
    sliderTemp->setValue(1);
    sliderTemp->setTickInterval(1);

    ui->mainToolBar->setBaseSize(800,36);

    calendar->hide();
    timeEdit->setDisplayFormat("hh:mm:ss");


    // инициализация сетевых клиентов для подключения к серверу потока ТС
    clientTcp = new ClientTcp(server_ipport, &logger, false, "АРМ ШН");
    QObject::connect(clientTcp, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    clientTcp->start();
//    scale = 1;


    // динамическое формирование расширенного тулбара
    // 1 - масштабирование
    // 2 - поиск изменений ТС
    // 3 - поиск ошибок связи
    ui->toolBar2->addWidget(new QLabel("Масштаб:  ", ui->toolBar2));
    ui->toolBar2->addWidget(sliderScale = new QSlider(Qt::Horizontal, ui->toolBar2));
    sliderScale->setMinimum(-50);
    sliderScale->setMaximum(100);
    sliderScale->setValue(0);
    sliderScale->setTickPosition(QSlider::TicksAbove);
    sliderScale->setFixedWidth(180);
    connect(sliderScale, SIGNAL(valueChanged(int)), this, SLOT(scaleView()));

    // 2 - флажок "Изменения ТС" и список ТС
    ui->toolBar2->addWidget(new QLabel("    ", ui->toolBar2));
    ui->toolBar2->addWidget(checkFindTs = new QCheckBox(" Изменение ТС", ui->toolBar2));
    checkFindTs->setLayoutDirection(Qt::RightToLeft);
    QObject::connect(checkFindTs, SIGNAL(toggled(bool)), SLOT(tsToggled(bool)));        // флажок ТС

    ui->toolBar2->addWidget(new QLabel("    ", ui->toolBar2));
    ui->toolBar2->addWidget(cmbTs = new QComboBox);
    cmbTs->setMaxVisibleItems(30);
    cmbTs->setEnabled(false);

    // 3 - флажок "Ош.связи"
    ui->toolBar2->addWidget(new QLabel("    ", ui->toolBar2));
    ui->toolBar2->addWidget(checkFindLink = new QCheckBox(" Ош.связи", ui->toolBar2));
    checkFindLink->setLayoutDirection(Qt::RightToLeft);

    ui->toolBar2->setVisible(false);

    on_actionBlackbox_triggered();

    foreach (Station * st, Station::Stations.values())
    {
        foreach (ShapeId * p, st->formList)
          {
              StationsCmb->addItem(p->Name(), qVariantFromValue((void *) p));
          }
    }
    QObject::connect(StationsCmb, SIGNAL(currentIndexChanged(int)), SLOT(stationSelected(int)));
    StationsCmb->model()->sort(0);
    StationsCmb->setCurrentIndex(0);                        // выбор первой станции в списке

    idTimer = startTimer(1000);
    arhDateTime = QDateTime::currentDateTime();
    reader = new ArhReader(pathTemp,"@_");

    Logger::LogStr ("Конструктор MainWindow завершил работу");
}

MainWindow::~MainWindow()
{
    Logger::LogStr ("Завершение работы. ~MainWindow()");

    delete clientTcp;
    delete reader;

    delete StationsCmb;
    delete dateEdit;
    delete calendar;
    delete stepValue;

    delete labelTemp;
    delete sliderTemp;
    delete labelTemp2;
    delete sliderScale;
    delete checkFindLink;

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


// выбор станции в списке
void MainWindow::stationSelected(int index)
{
    Q_UNUSED(index)
    ShapeId * shapeId = (ShapeId *)StationsCmb->currentData().value<void *>();
    g_actualStation = shapeId->St();
    setCentralWidget(child = new ShapeChild(shapeId->Set()));
    scaleView();

    cmbTs->clear();
    foreach (Ts *ts, g_actualStation->TsSorted)
        cmbTs->addItem(ts->Name());
    cmbTs->setCurrentIndex(-1);

    emit changeStation(g_actualStation);
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
    if (!blackBoxMode)
    {
        DDataFromMonitor * pDtMntr = (DDataFromMonitor *)client->rawData();
        pDtMntr->Extract(client->rawLength());
    }
}

// получены необрамленные данные - отдельный сигнал
void MainWindow::rawdataready(ClientTcp *client)
{
    ui->statusBar->showMessage(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->rawLength()), 10000);
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
    if (dlgSvtf == nullptr)
    {
        dlgSvtf = new DlgSvtfInfo(g_actualStation, this);
        dlgSvtf->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgSvtf, SLOT(changeStation(Station*)));
    }
    else
        dlgSvtf->setVisible(!dlgSvtf->isVisible());
}

void MainWindow::on_action_Stations_triggered()
{
    if (dlgStations == nullptr)
    {
        dlgStations = new DlgStationsInfo(this);
        dlgStations->show();
    }
    else
        dlgStations->setVisible(!dlgStations->isVisible());
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

// вкл/откл тулбар
void MainWindow::on_action_Toolbar_triggered()
{
    ui->mainToolBar->setVisible(ui->action_Toolbar->isChecked());
}

// вкл/откл панели масштабирования
void MainWindow::on_action_ToolBar2_triggered()
{
    ui->toolBar2->setVisible(ui->action_ToolBar2->isChecked());
}

QProcess process;
// протокол работы модуля
void MainWindow::on_action_15_triggered()
{
    QStringList params;
    params << logger.GetActualFile();
    process.start(editor, params);
//    process.waitForFinished(-1);
}


// --------------------------------------------- Работа с архивом ----------------------------
//                                           можно вынести в отд.файл
// щелчок флажка поиск изменений ТС; если сняли флажок - очистить поле выбора искомого ТС в списке
void MainWindow::tsToggled(bool checked)
{
    cmbTs->setEnabled(checked);
    if (!checked)
        cmbTs->setCurrentIndex(-1);
    else
        cmbTs->showPopup();
}


void MainWindow::on_actionBlackbox_triggered()
{
    blackBoxMode = ui->actionBlackbox->isChecked();
    dateEdit         ->setEnabled(blackBoxMode);
    timeEdit         ->setEnabled(blackBoxMode);
    ui->actionPrev   ->setEnabled(blackBoxMode);
    ui->actionReverce->setEnabled(blackBoxMode);
    ui->actionPlay   ->setEnabled(blackBoxMode);
    ui->actionNext   ->setEnabled(blackBoxMode);
    ui->action_Stop  ->setEnabled(blackBoxMode);
    stepValue        ->setEnabled(blackBoxMode);
    labelStep        ->setEnabled(blackBoxMode);
    labelTemp        ->setEnabled(blackBoxMode);
    labelTemp2       ->setEnabled(blackBoxMode);
    sliderTemp       ->setEnabled(blackBoxMode);
    checkFindTs      ->setEnabled(blackBoxMode);
    cmbTs            ->setEnabled(blackBoxMode && checkFindTs->isChecked());
    checkFindLink    ->setEnabled(blackBoxMode);
    if (blackBoxMode)
    {
        dateEdit->setDate(arhDateTime.date());
        timeEdit->setTime(arhDateTime.time());
    }
}

// нажатие кнопки "Воспроизведение" (">")
// надо воспроизводить архив с заданного времени
void MainWindow::on_actionPlay_triggered()
{
    bPlay = ui->actionPlay->isChecked();
    if (bPlay)
    {
        // где-то здесь надо сделать анализ наличия файла данных и реальной даты ,
        // и, в случае несовпадения - поиск и разархивирование сжатого архива, и, при необходимости, -
        // предварительную подкачку архивного файла с сервера

        arhDateTime = QDateTime(dateEdit->date(),timeEdit->time());
        reader->setArhName(arhDateTime);                    // определение файла для чтения и инициализация
        //int ret = reader->Read(arhDateTime);
        int ret = readNext(&arhDateTime);
        if (ret == -1)
        {
            // не нашли; проблема с датой, нужна подкачка нужного файла
            ui->actionPlay->setChecked(false);
            bPlay = false;
            return;
        }

        bPlayBack = false;
        ui->actionNext->setEnabled(false);
        ui->actionPrev->setEnabled(false);
        ui->actionReverce->setEnabled(false);
        ui->action_Stop->setEnabled(true);
        dateEdit->setEnabled(false);
        timeEdit->setEnabled(false);
    }
    else
        on_action_Stop_triggered();
}

// нажатие кнопки "СТОП" ("||")
void MainWindow::on_action_Stop_triggered()
{
    ui->actionPlay->setChecked(false);
    bPlay = bPlayBack = false;
    ui->actionNext->setEnabled(true);
    ui->actionPrev->setEnabled(true);
    ui->actionReverce->setEnabled(true);
    ui->action_Stop->setEnabled(false);

    // killTimer(idTimer);
    dateEdit->setEnabled(true);
    timeEdit->setEnabled(true);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    //qDebug() << "Таймер" << event->timerId();
    if (blackBoxMode)
    {
        if (bPlay)
            readNext();
        else
        if (bPlayBack)
            readPrev();
    }
    else
    {
        dateEdit->setDate(QDate::currentDate());
        timeEdit->setTime(QTime::currentTime());
    }

    // отслеживаем сосотояние диалогов и приводим в соотвествие состояние опций меню
    ui->action_TS       ->setChecked(dlgTs      != nullptr && dlgTs     ->isVisible());
    ui->action_TU       ->setChecked(dlgTu      != nullptr && dlgTu     ->isVisible());
    ui->action_Routes   ->setChecked(dlgRoutes  != nullptr && dlgRoutes ->isVisible());
    ui->action_RC       ->setChecked(dlgRc      != nullptr && dlgRc     ->isVisible());
    ui->action_STRL     ->setChecked(dlgStrl    != nullptr && dlgStrl   ->isVisible());
    ui->action_SVTF     ->setChecked(dlgSvtf    != nullptr && dlgSvtf   ->isVisible());
    ui->action_Stations ->setChecked(dlgStations!= nullptr && dlgStations->isVisible());
    ui->action_KP       ->setChecked(dlgKp      != nullptr && dlgKp     ->isVisible());
    //    ui->action_SVTF     ->setChecked(dlgKp != nullptr && dlgKp->isVisible());
    //    ui->on_action_OTU   ->setChecked(dlgKp != nullptr && dlgKp->isVisible());

}

// прочитать и отобразить след.запись в архиве
// если задан шаг - смещение на заданное время
// если не заданы условия поиска - смещаемся вплоть до любого изменения состояния ТС или связи на станции
// если задано условие поиска (ТС или связь) - ищем изменение конкретного ТС или состояния связи
bool MainWindow::readNext(QDateTime* rqDate, bool findChanges)     // =false
{
    int sts;
    int ret = 0;

    if (findChanges && isFindTsChanges())
        sts = g_actualStation->GetTsStsByNameEx(cmbTs->currentText());

    qDebug() << "Следующая запись";
    if (reader != nullptr)
    {
        Station::FastScanArchive = isExtFind();
        while (true)
        {
            // если задан шаг смещения - смещение на заданное задатчиком время dt
            int dt = stepValue->value();
            if (dt > 0)
                ret = reader->Read(QDateTime::fromTime_t(reader->time() + dt*60));  // время с учетом задатчика
            else
            // если не задан шаг задатчиком dt, но в функцию передано конкретное требуемое время rqDate - пытаемся позиционируовать на это время
            if (rqDate != nullptr)
                ret = reader->Read(*rqDate);                                        // явно заданное время
            // если время не задано и шаг не задан - просто читаем след.запись
            else
                ret = reader->Next();                                               // след.запись

            // не нашли в текущем файле нужного времени
            if (ret==-1)
            {
                // надо переместить время на начало следующего часа в случае окончания предыдущего
                // можно проверить dt на допустимое значение
                if (reader->isExist() && reader->isEndOfFile())
                {
                    if (reader->setNextHour(QDateTime::fromTime_t(reader->time())))
                    {
                        qDebug() << reader->getFileName();
                        ret = reader->First();
                    }
                }
                else
                {
                    QString zipFile = pathSave + reader->getZipName(reader->rqt());
                    if (QFile::exists(zipFile))
                    {
                        qDebug() << "Извлекаем файл из архива " << zipFile;
                        reader->close();
                        QProcess process;
                        QStringList params;
                        params << "-o" << zipFile << reader->getFileName() << "-d" << pathTemp;
                        process.start(decompressor, params);
                        if (process.waitForFinished())
                        {
                            reader->setArhName(reader->rqt());
                            if ((ret =reader->First()) ==-1)
                            {
                                // чужое время, лажа; запросить архив снова
                            }
                        }
                        else
                        {
                            // проблемы завершения процесса разархивирования
                        }
                    }
                    else
                    {
                        // подкачка
                        break;
                    }
                }
                // if (нет след файла)
                // {
                //     if (нет архива)
                //        подкачка (если нет возможности подкачки - уведомление о фатальной ошибке)
                //     разархивирование (если нет возможности разархивирования - уведомление о фатальной ошибке))
                // }
                // читаем первую запись
                // if (ошибка)
                //     уведомление о невозможности

                // если ничего никак не нашли (ret=-1) и это не поиск изменений - выход
                if (ret < 0 || !findChanges)
                    break;
            }

            // обработать данные
            DDataFromMonitor * data = (DDataFromMonitor *)reader->Data();
            QDateTime t = QDateTime::fromTime_t(reader->time());
            dateEdit->setDate(t.date());
            timeEdit->setTime(t.time());
            data->Extract(reader->Length());
            QApplication::processEvents();

            // если шаг по времени или не ищем изменения или изменения есть - выход из цикла
            if (dt || !findChanges || (!isExtFind() && g_actualStation->IsTsChanged()))
                break;                                      // нашли измененные ТС
            // поиск изменения статуса связи
            if (findChanges && isFindLinkErrors() && g_actualStation->IsLinkStatusChanged())
                break;
            if (findChanges && isFindTsChanges() && sts != g_actualStation->GetTsStsByNameEx(cmbTs->currentText()))
                 break;
        }

        if (Station::FastScanArchive)
        {
            Station::FastScanArchive = false;
            g_actualStation->AcceptTS();
        }
    }
    return ret > 0;
}

// прочитать и отобразить пред.запись в архиве
void MainWindow::readPrev()
{
    qDebug() << "Предыдущая запись";
}


// обработка кнопки шаг вперед
void MainWindow::on_actionNext_triggered()
{
    if (!readNext(nullptr, true))
    {
        // не согли найти очередной архив
    }
}

// обработка кнопки шаг назад
void MainWindow::on_actionPrev_triggered()
{

}

// О программе
void MainWindow::on_action_About_triggered()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );
    QMessageBox::about(this, "О программе", QString("ДЦ ЮГ. АРМ ШН\n%1\n\nФайл: %2.\nДата сборки: %3\n© ООО НПЦ Промавтоматика, 2016").arg(version).arg(info.filePath()).arg(info.created().toString(FORMAT_DATETIME)));
}

void MainWindow::on_action_QtAbout_triggered()
{
    QMessageBox::aboutQt(this, "Версия QT");
}
