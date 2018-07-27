#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "../common/inireader.h"
#include "../common/sqlmessage.h"
#include "../common/sqlserver.h"
#include "../common/sqlblackbox.h"
#include "../spr/krug.h"
#include "../spr/esr.h"
#include "../spr/station.h"
#include "../spr/raspacker.h"
#include "../forms/dlgkpinfo.h"
#include "../spr/stationnetts.h"
#include "../spr/stationnettu.h"

// Блок глобальных переменных проекта ==========================================================================================================================
std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения
std::mutex sendMutex;                                       // мьютекс для доступа к waterMsg
std::condition_variable waterMsg;                           // Ожидаем обработки сообщения

MainWindow * MainWindow::mainWnd;                           // статический экземпляр главного окна

QString version = "1.0.1";                                  // версия приложения
QString title = "ДЦ ЮГ на базе КП Круг. Станция связи";

QString mainCom,                                            // порт прямого канала
        rsrvCom;                                            // порт обводного канала
QString configMain,                                         // строка конфигурации BlockingRS прямого канала
        configRsrv;                                         // строка конфигурации BlockingRS обратного канала
int     baud        = 57600;                                // скорость обмена с модемами
int     delay       = 10;                                   // минимальная задержка между опросами, мс
int     breakdelay  = 50;                                   // максимально допустимый интервал между байтами в пакете, мс
quint64 driftCount  = 0;                                    // число корректных пакетов, принятых с чужого адреса

QString path;

// коммутация каналов связи (используется в конфигурациях, когда основная и резервная РСС имеют разные IP и включены одновременно
// отображение:
// - индикатор О/Р - основная/резервная; активная РСС - зеленый цвет, пассивная - белый
// - флажок ОТКЛ включен на активной РСС
//   при наличии аппаратного коммутатора название соответствует типу РСС: ОСНОВ/РЕЗЕРВ
//   при вкл/откл флажка выдаетсязапрос подтверждения соответствующег действия
bool    mainRss = true;                                     // основная станция связи (опция MAINRSS)
QString nextRssIP;                                          // ip-адрес смежной станции связи (резервной или основной)
int     nextRssPort;                                        // номер порта смежной станции связи (резервной или основной)
bool    hardSwith = false;                                  // наличие аппаратного пкоммутатора
bool    hardSwitchAuto = true;                              // автопереключение

#ifdef Q_OS_WIN
    QString editor = "notepad.exe";                         // блокнот
#endif
#ifdef Q_OS_MAC
    QString editor = "TextEdit";                            // блокнот
#endif
#ifdef Q_OS_LINUX
    QString editor = "gedit";                               // блокнот
#endif
    Logger logger("log/mdmagent.log", true, true);

    QString images(":/status/images/");                     // путь к образам status/images
    QString imagesEx(":/images/images/");                   // путь к образам images/images

    QString dbname      = "bd/arm.db";
    QString esrdbbname  = "bd/arm.db";
    QString extDb       = "bd/armext.db";
    QString iniFile     = "mdmagent.ini";

    // строки подключения к основному и резервному SQL-серверам
    QString mainSql = "";                                   // "DRIVER=QPSQL;Host=192.168.0.105;PORT=5432;DATABASE=blackbox;USER=postgres;PWD=358956";
    QString rsrvSql = "";

// =============================================================================================================================================================



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWnd = this;
    modulType=APP_MDMAGENT;                                 // тип приложения
    QString tmp;

    // --------------------------------------------------------------------------------------------------------------------------
    // блокируем мьютекс завершения работы
    exit_lock.lock();

    path = QDir::currentPath();

    // если ini-файл задан параметром командной строки, используем
    QStringList list = QCoreApplication::arguments();
    if (list.count() > 1)
        iniFile = list[1];

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");
    Logger::LogStr ("Конфигурация: " + iniFile);

    IniReader rdr(iniFile);

    rdr.GetText("DBNAME", dbname);
    dbname = path + "/" + dbname;
    path = QFileInfo(dbname).absoluteDir().absolutePath();
    extDb       = path + "/bd/armext.db";
    esrdbbname  = path + "/bd/arm.db";
    portSnd = 0;                                                // порт передачи датаграмм
    portRcv = 0;                                                // порт приема датаграмм


    int ras = 1;
    rdr.GetInt("KRUG"    , ras   );                             // номер станции связи
    portTcp = 1002;                                             // по умолчанию порт 1002
    rdr.GetInt("TCPPORT" , portTcp);                            // TCP-порт сервера входящих подключений модулей УПРАВЛЕНИЕ
    rdr.GetText("MAIN"   , mainCom);                            // порт прямого   канала
    rdr.GetText("RESERVE", rsrvCom);                            // порт обратного канала
    rdr.GetInt("ВAUD"    , baud  );                             // скорость обмена с модемами
    rdr.GetInt("DELAY"   , delay );                             // минимальная задержка между опросами станций
    rdr.GetInt("READ_INTERVAL", breakdelay);                    // максимально допустимый интервал между байтами в пакете, мс

    rdr.GetInt("UDPSEND"   , portSnd);                          // порт передачи датаграмм
    rdr.GetInt("UDPRECEIVE", portRcv);                          // порт приема датаграмм

    rdr.GetText("SQLSERVERMAIN", mainSql);                      // строка подключения к основному  SQL (postgresql)
    rdr.GetText("SQLSERVERRSRV", rsrvSql);                      // строка подключения к резервному SQL (postgresql)

    rdr.GetBool("MAINRSS", mainRss);                            // MAINRSS - ОN/OFF - основная/резервная
    if (rdr.GetText("LINKRSS", tmp))                            // LINKRSS=192.168.0.101:7005
        TcpHeader::ParseIpPort(tmp, nextRssIP, nextRssPort);
    rdr.GetBool("HARDWARESWITCH", hardSwith);                   // HARDWARESWITCH=OFF
    rdr.GetBool("AUTOSWITCH", hardSwitchAuto);                  // AUTOSWITCH

    // --------------------------------------------------------------------------------------------------------------------------

    KrugInfo * krug = nullptr;
//    Esr::ReadBd(esrdbbname, logger);                            // ЕСР
    Station::ReadBd(dbname, krug, logger, QString("WHERE RAS = %1 ORDER BY Addr").arg(ras));                      // станции

    loadResources();

    blackbox = new SqlBlackBox(mainSql, rsrvSql, &logger);
    blackbox->SqlBlackBox::putMsg(0, "Запуск", APP_MDMAGENT, LOG_NOTIFY);

    // формируем представление КП станций в основном окне в несколько строк
    // в качестве входного массива используем вектор станций StationsOrg, отсортированный по заданному при чтении БД критерию "Addr"
    int row = 0, col = 0, colmax = 13;                          // не более 13 станций в одной строке
    for (auto st : Station::StationsOrg)
    {
        kpframe * kp = new kpframe(this, st);                   // создаем класс
        // связываем событие выбора станции и слот SelectStation
        connect(kp, SIGNAL(SelectStation(Station*)), this, SLOT(SelectStation(Station*)));
        ui->gridLayout_kp->addWidget(kp,row,col++);
        if (col==colmax)
        {
            col = 0;
            row++;
        }
    }

    // признак основной/резервный
    ui->frame_mainBM->setRsrv(false);
    ui->frame_rsrvBM->setRsrv(true );

    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);

    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Qt::darkGreen);
    ui->label_or->setPalette(pal);


    // индикатор БПДК/УПОК: изначально серый
    // если принят пакет с данными ОТУ - моргнуть ярко-зеленым, обновить засечку времени
    // периодически (по таймеру) проверять на молчание более 1 мин: если засечки были, но молчит долго - желтый, иначе темно зеленый.
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::gray, Qt::darkGray);

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(int,void*,void*)), this, SLOT(GetMsg(int,void*,void*)));

    // синхронизация состояния флажка "С квитанцией" с переменной g_rqAck
    g_rqAck = ui->checkBox_ack->isChecked();

    // синхронизация состояния флажка "Полный опрос" с переменной Station::bFullPollingAll
    Station::bFullPollingAll = ui->checkBox_Full->isChecked();

    // конфигурация портов основного и обратного канала задаются в строках configMain, configRsrv
    configMain = QString("%1,%2,N,8,1").arg(mainCom).arg(baud);

ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on);
ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::off);

    ui->checkBox_Main->setChecked(configMain.length() > 0);
    ui->checkBox_Main->setEnabled(configMain.length() > 0);
    ui->checkBox_Rsrv->setChecked(configRsrv.length() > 0);
    ui->checkBox_Rsrv->setEnabled(configRsrv.length() > 0);

    // запускаем рабочий поток опроса каналов
    pThreadPolling    = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling, (long)this));

    if (Station::StationsOrg.size() > 0)
        SelectStation(Station::StationsOrg[0]);

    dlgKp = nullptr;

    // индикатор квитанций
    ui->label_ack->set(QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);

    ui->label_port->setText(QString("Порт: %1").arg(portTcp));

    QTableWidget * t = ui->tableWidget;
    t->setColumnCount(1);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Клиенты" /*<< "Порт"*/);
    t->resizeColumnsToContents();

        // автоматически растягтваем 1-й столбец
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // создание сервера TCP-подключений и привязка сигналов к слотам
    server = new ServerTcp(portTcp, QHostAddress("192.168.0.105"), &logger);
    QObject::connect(server, SIGNAL(acceptError  (ClientTcp*  )), this, SLOT(slotAcceptError      (ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*  )), this, SLOT(slotSvrNewConnection (ClientTcp*)));
    QObject::connect(server, SIGNAL(dataready    (ClientTcp*  )), this, SLOT(slotSvrDataready     (ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected (ClientTcp*  )), this, SLOT(slotSvrDisconnected  (ClientTcp*)));
    QObject::connect(server, SIGNAL(roger        (ClientTcp*  )), this, SLOT(slotRoger            (ClientTcp*)));
    server->start();

    // если определены сетевые порты, создаем сокеты для передачи/приема датаграмм
    if (IsNetSupported())
    {
        Logger::LogStr ("Создаем UDP-сокеты для информационного обмена");
        sndSocket = new QUdpSocket(this);              // сокет для передачи в КП датаграмм
//      sndSocket->bind(QHostAddress("192.168.0.105"));// можно привязать сокет к конкретному сетевому адаптеру

        rcvSocket = new QUdpSocket(this);              // сокет для приема датаграмм из КП
        rcvSocket->bind(QHostAddress::AnyIPv4, portRcv, QUdpSocket::ShareAddress);  // привязать сокет ко всем IP и порту
        connect(rcvSocket, SIGNAL(readyRead()), this, SLOT(readDatagramm()));
    }
    else
        rcvSocket = sndSocket = nullptr;

    tUcSnd = 0;                                                 // засечка передачи из УЦ
    startTimer (1000);

}

MainWindow::~MainWindow()
{
    Logger::LogStr ("Деструктор MainWindow. Освобождаем ресурсы");
    blackbox->SqlBlackBox::putMsg(0, "Завершение работы", APP_MDMAGENT, LOG_NOTIFY);
    exit_lock.unlock();

    if (blackbox != nullptr)
        delete blackbox;
    Station::Release();

    delete ui;
}

// загрузка графических ресурсов
void MainWindow::loadResources()
{
    Logger::LogStr ("Загрузка графических ресурсов");
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

    //tooltip = true;
}


// смена активной станции
// - убрать выделение с пассивных
// - установить выделение активной
// - дать информацию по активной
void MainWindow::SelectStation(class Station * actualSt)
{
    ui->label_stName->setText(actualSt->Name());
    for (auto st : Station::StationsOrg)
    {
        this->actualSt = actualSt;
        kpframe * kp = (kpframe *)st->userData;
        kp->SelectSt(actualSt==st);
        if (actualSt==st)
        {
            ui->frame_mainBM->Show(st);
            ui->frame_rsrvBM->Show(st);
            emit changeStation(st);
        }
    }
}

// запрос на закрытие приложения
void MainWindow::closeEvent(QCloseEvent * e)
{
    if (QMessageBox::question(this, title, "Завершить работу станции связи?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        e->ignore();
    waterMsg.notify_all();
}


// Формирование и передача директив управления КП из панели управления ============================================================================

// отключить основной на заданное время
void MainWindow::on_pushButtonMainOff_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        QString msg =   "Введена команда БЕЗУСЛОВНОГО перехода на резервный блок КП."
                        "Если резернный блок неисправен, Вы можете ПОТЕРЯТЬ КОНТРОЛЬ СТАНЦИИ!\n"
                        "Рекомендуется сначала убедиться в его работоспособности путем отключения основного блока"
                        "с ограничением времени на 1-3 минуты\n\nВСЕ РАВНО ВЫПОЛНИТЬ КОМАНДУ ?";

        WORD delay = (WORD)ui->spinBox->value();
        if (delay > 0 ||QMessageBox::question(this, title, msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            BYTE buf[sizeof(TuPackBypassMain)];
            memmove(buf, TuPackBypassMain, sizeof(buf));
            * (WORD *)(buf + sizeof (buf) - sizeof (WORD)) = delay;
            actualSt->AcceptDNC((RasData*)&buf);

            QString msg = QString("Ст.%1. Команда из панели РСС: ").arg(actualSt->Name()) + (delay ? QString("Отключение основного блока на %1 мин.").arg(delay) : "Отключение основного блока");
            Logger::LogStr (msg);
            blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
            QMessageBox::information(this, title, msg, QMessageBox::Ok);
        }
    }
}

// отключить резервный на заданное время
void MainWindow::on_pushButtonRsrvOff_clicked()
{
    WORD delay = (WORD)ui->spinBox->value();
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        BYTE buf[sizeof(TuPackBypassRsrv)];
        memmove(buf, TuPackBypassRsrv, sizeof(buf));
        * (WORD *)(buf + sizeof (buf) - sizeof (WORD)) = delay;
        actualSt->AcceptDNC((RasData*)&buf);

        QString msg = QString("Ст.%1. Команда из панели РСС: ").arg(actualSt->Name()) + (delay ? QString("Отключение резервного блока на %1 мин.").arg(delay) : "Отключение резервного блока");
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// включить основной блок безусловно
void MainWindow::on_pushButtonToMain_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackSetMain);
        QString msg = QString("Ст.%1. Команда из панели РСС: Включение основного блока").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// Переход на резервный блок безусловно
void MainWindow::on_pushButtonToRsrv_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        QString msg = "Введена команда БЕЗУСЛОВНОГО перехода на резервный блок КП."
                      "Если резернный блок неисправен, Вы можете ПОТЕРЯТЬ КОНТРОЛЬ СТАНЦИИ!\n"
                      "Рекомендуется сначала убедиться в его работоспособности путем отключения основного блока"
                      "на 1-3 минуты командой 'Отключение основного блока'\n\nВСЕ РАВНО ВЫПОЛНИТЬ КОМАНДУ ?";
        if (QMessageBox::question(this, title, msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            actualSt->AcceptDNC((RasData*)&TuPackSetRsrv);
            QString msg = QString("Ст.%1. Команда из панели РСС: Переход на резервный блок").arg(actualSt->Name());
            Logger::LogStr (msg);
            blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
            QMessageBox::information(this, title, msg, QMessageBox::Ok);
        }
    }
}

// запуск теста МТУ/МТС
void MainWindow::on_pushButtonTest_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackTestMtuMts);
        QString msg = QString("Ст.%1. Команда из панели РСС: Запуск теста МТУ/МТС").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// Сброс АТУ
void MainWindow::on_pushButtonATU_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackResetAtu);
        QString msg = QString("Ст.%1. Команда из панели РСС: Сброс АТУ").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
     }
}

// перезапуск КП
void MainWindow::on_pushButtonReset_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty()
        && QMessageBox::question(this, title, "Выполнить перезапуск КП", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes
       )
    {
        actualSt->AcceptDNC((RasData*)&TuPackRestart);
        QString msg = QString("Ст.%1. Команда из панели РСС: Перезапуск КП").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// запрос числа перезапусков
void MainWindow::on_pushButtonGetReconnect_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackRestartCount);
        QString msg = QString("Ст.%1. Команда из панели РСС: Запрос числа перезапусков КП").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// кратковременно отключить питание активного (холодный рестарт)
void MainWindow::on_pushButtonResetMain_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackResetOne);
        QString msg = QString("Ст.%1. Команда из панели РСС: Отключить питание активного блока (холодный рестарит блока)").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// кратковременно отключить питание соседнего (холодный рестарт)
void MainWindow::on_pushButtonResetRsrv_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackResetAnother);
        QString msg = QString("Ст.%1. Команда из панели РСС: Отключить питание смежного неактивного блока (холодный рестарит блока)").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}

// вкл.сторожевой таймер
void MainWindow::on_pushButtonWatchdog_clicked()
{
    if (actualSt != nullptr && actualSt->IsTuEmpty())
    {
        actualSt->AcceptDNC((RasData*)&TuPackWatchdogOn);
        QString msg = QString("Ст.%1. Команда из панели РСС: Включить сторожевой таймер)").arg(actualSt->Name());
        Logger::LogStr (msg);
        blackbox->SqlBlackBox::putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
        QMessageBox::information(this, title, msg, QMessageBox::Ok);
    }
}
// ===============================================================================================================================================================


// обработка событий GUI

// обработка тика таймера
void MainWindow::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    // управление индикатором УЦ СПОК/УПОК: если молчит более 60 сек - желтый
    if (tUcSnd && (QDateTime::currentDateTime().toTime_t() - tUcSnd  > 60))
        ui->label_OTU->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow, Qt::darkGreen);

    // проверяем "зависшие" данные в выходных очередях, и если такие есть - удаляем
    // ситуация крайне маловероятная, тем не менее
    // можно выдать блок данных в HEX
    for (auto st : Station::StationsOrg)
    {
        RasData * data = st->GetRasDataOut();
        if (data->Length() && st->tuTime.secsTo(QDateTime::currentDateTime()) > 30 )
        {
            std::lock_guard <std::mutex> locker(st->GetOutDataMtx());
            data->Clear();                                // очищаем буфер линейных данных, если они старые
            Logger::LogStr(QString("Ст.%1. Удаляем устаревшие данные для КП из очереди").arg(st->Name()));
        }
    }
}

// изменение состояния флажка "С квитанцией"
void MainWindow::on_checkBox_ack_stateChanged(int arg1)
{
    g_rqAck = arg1 > 0;
    if (!g_rqAck)
        waterAck.notify_all();
}

// изменение флага "Полный опрос"
void MainWindow::on_checkBox_Full_stateChanged(int arg1)
{
    Station::bFullPollingAll = arg1 > 0;
}

// отобразить число циклов
void MainWindow::setCycles(unsigned int n)
{
    ui->label_cycles->setText(QString::number(n));
}

// отобразить длит.цикла
void MainWindow::setPeriod(unsigned int n)
{
    ui->label_time->setText(QString::number(n));
}

void MainWindow::on_action_KP_triggered()
{
    if (dlgKp==nullptr)
    {
        dlgKp = new DlgKPinfo(actualSt, this);
        dlgKp->show();
        QObject::connect(this, SIGNAL(changeStation(Station*)), dlgKp, SLOT(changeStation(Station*)));
    }
    else
        dlgKp->setVisible(!dlgKp->isVisible());
}



// ===============================================================================================================================================================
// слотй уведомлений сервера

// ошибка на сокете
void MainWindow::slotAcceptError(ClientTcp * conn)
{
    Q_UNUSED(conn);
}

// подключен новый клиент: добавляем строку в таблицу клиентов
void MainWindow::slotSvrNewConnection (ClientTcp *conn)
{
    QTableWidget * t = ui->tableWidget;
    int row = t->rowCount();
    t->setRowCount( row + 1);
    t->setItem(row,0, new QTableWidgetItem (conn->name()));
    t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)conn));     // запомним переезд
}

// отключен подключенный ранее клиент: ищем в таблице клиентов строку по conn и удаляем
void MainWindow::slotSvrDisconnected  (ClientTcp * conn)
{
    QTableWidget * t = ui->tableWidget;
    int row = -1;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        ClientTcp * p = (ClientTcp *) item->data(Qt::UserRole).value<void*>();
        if (conn==p)
        {
            row = i;
            break;
        }
    }
    t->removeRow(row);
}

// получены данные от модуля Управление
void MainWindow::slotSvrDataready     (ClientTcp * conn)
{
    QString name(conn->name());
    QString s("Приняты данные от клиента " + conn->name());
    Logger::LogStr (s);
/*
    // проверить список допустимых ip для управления (опция ENABLETUIP)
    if (!IsTuIpEnabled(conn->remoteIP())
    {
        Logger::LogStr(QString());
        return;
    }
*/
    StationNetTU * ptu = (StationNetTU *) conn->rawData();
    RasData * data = (RasData *) ptu->data;


    Station * st = Station::GetById(ptu->nost);
    if (st!=nullptr)
    {
        ui->statusBar->showMessage(QString("%1. АРМ ДНЦ -> %2:   %3").arg(QDateTime::currentDateTime().toString(FORMAT_TIME)).arg(st->Name()).arg(data->About()));

        // если есть данные ОТУ, моргнуть индикатором ОТУ и засечь время последней работоспособности УЦ

        st->AcceptDNC(data);

        // если были данные БПДК/УПОК, моргнуть индикатором УЦ
        if (data->LengthOtu())
        {
            ui->label_OTU->set (QLed::ledShape::box, QLed::ledStatus::blink_once, Qt::green, Qt::darkGreen);
            tUcSnd = QDateTime::currentDateTime().toTime_t();
        }
    }
    else
    {
        // некорректный формат
    }

}

// получена квитанция от АРМ ДНЦ
void MainWindow::slotRoger  (ClientTcp * client)
{
    armAcked = true;
    waterAck.notify_all();
    Q_UNUSED (client)
    ui->label_ack->set(QLed::ledShape::box, QLed::ledStatus::on, Qt::green);
}

// отправить данные всем подключенным клиентам, от которыъ было подтверждение
void MainWindow::sendToAllClients(StationNetTS* info)
{
    armAcked = false;
    server->sendToAll((char *) info, info->length);
    ui->label_ack->set(QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
}


// ===============================================================================================================================================================
// работа с датаграммами

// слот "прием датаграмм" от КП
void MainWindow::readDatagramm()
{
    while (rcvSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(rcvSocket->pendingDatagramSize());
        rcvSocket->readDatagram(datagram.data(), datagram.size());

        // зписываем принятые данные в очередь
        {
        std::lock_guard <std::mutex> locker(mtxDataNet);
        for (int i=0; i<datagram.size(); i++)
            dataInNet.push(datagram[i]);
        }

        // уведомляем о приеме ожидающий рабочий поток
        waterNet.notify_all();

    }
}

// поддержка сети
bool MainWindow::IsNetSupported()
{
    return mainWnd->portSnd && mainWnd->portRcv;
}


// ===============================================================================================================================================================
// обработка сообщений, генерируемых потоком ThreadPolling
void MainWindow::GetMsg (int np, void * param, void * param2)
{
    switch (np)
    {
        case MSG_LOG:                                           // лог сообщения
            {
            Logger::LogStr (*(QString *)param);                 // лог
            ui->statusBar->showMessage(*(QString *)param);      // GUI - строка состояния окна
            }
            break;

        case MSG_SHOW_INFO:                                     // отобразить информацию об опрошенной станции
            ((kpframe *)param)->Show();
            break;

        case MSG_SHOW_PING:                                     // отобразить информацию о точке опроса
            ((kpframe *)param)->SetActual(true,false, param2 != nullptr);
            break;

        case MSG_SHOW_SND:                                      // отобразить информацию о передаче запроса в КП
            {                                                   // сообщение отправляется из TryOneChannel(...)
            RasPacker * pack = (RasPacker*)param;
            Station * st = pack->st;                            // указатель ДОЛЖЕН быть определен
            ui->label_Snd->setText(QString("%1  -> %2").arg(Logger::GetHex(param, pack->Length())).arg(st->Name()));
            ui->label_cntsnd->setText(QString::number(pack->Length()));

            // отладочное сообщение в строке сообщений
            ui->statusBar->showMessage(QString("%1. Передача данных ст. %2").arg(QDateTime::currentDateTime().toString(FORMAT_TIME)).arg(st->Name()));

            // если есть информация ОТУ - моржок индикатором ОТУ
            RasData * p = pack->GetRasData();
            if (!pack->IsEmpty() && p->LengthOtu())
            {
                ((kpframe *)st->userData)->BlinkOtu();          // моргаем тем цветом, который есть
                ((kpframe *)st->userData)->SetOtuLed(st->IsOtuLineOk(), true);   // Моргнуть ОТУ если есть
            }

            setCycles(cycles);
            setPeriod((int)(start.msecsTo(QTime::currentTime())));
            }
            break;


        // успешный прием данных от КП; данные уже скопированы в rasDataIn
        // 1- моргнуть ОТУ если есть
        // 2- отобразить инфо блок
        // 3- обработать системную информацию с учетом особенностей разных версий КП и обновить изображение КП
        // 4- сформировать и передать данные подключенным клиентам
        // 5- отобразить состояние актуальной станции
        case MSG_SHOW_RCV:
            if (param!=nullptr)
            {
                // 1- моргнуть ОТУ если есть
                RasPacker* pack = (RasPacker* )param2;          // pack - весь пакет
                Station * actualst = (Station *)param;          // гарантированно не nullptr
                Station * st = pack->st;                        // станция с полученными данными
                RasData * data = st->GetRasDataIn();            // используем копию полученных данных в самом классе

                // если пакет заблудился (адрес источника = 0 или адрес назначения не 0, или станция не найдена - лог и выход)
                if (pack->src == 0 || pack->dst != 0 || st==nullptr )
                {
                    Logger::LogStr(QString("Пакет данных не предназначен РСС, данные игнорируются: %1").arg(Logger::GetHex(pack, pack->Length())));
                    break;
                }
                if (st != actualst)
                {
                    Logger::LogStr(QString("Прием данных от другой станции: %1 вместо %2").arg(st->Name()).arg(actualst->Name()));
                    ui->label_Drift->setText(QString::number(++driftCount));
                }

                // если отклик от ОМУЛ-а - засечка приема
                if (data->LengthOtu())
                {
                    st->FixOtuRcv();                            // засечка времени приема
                    ((kpframe *)st->userData)->SetOtuLed(true, true);   // Моргнуть ОТУ если есть
                }

                // 2- отобразить инфо блок
                ui->label_RCV->setText(QString(" %1 :    %2...<- %3").arg(data->About()).arg(Logger::GetHex(param2, std::min(48,(pack->Length())))).arg(st->Name()));
                ui->statusBar->showMessage(QString("%1. Прием данных по ст. %2").arg(QDateTime::currentDateTime().toString(FORMAT_TIME)).arg(st->Name()));
                ui->label_cntrcv->setText(QString::number(pack->Length()));

                // 3- обработать системную информацию с учетом особенностей разных версий КП и обновить изображение КП
                if (data->LengthSys())
                {
                    // учесть переменную длину 9/15/30 и в случае 30 - записать оба блока
                    // первым всегда идет активный блок
                    st->SetRsrv((data->PtrSys()[8] & 1) > 0);   // признак резерва
                    SysInfo * sysinfo = st->GetSysInfo(st->IsRsrv());
                    sysinfo->Parse(data->PtrSys(), data->LengthSys());

                    if (data->LengthSys() == 30)
                    {
                        // нформация по 2-м блокам: основному и резервному
                        sysinfo = st->GetSysInfo(!st->IsRsrv());
                        sysinfo->Parse(data->PtrSys()+15, 15);
                    }

                    // отрисовка актуального КП, включая индикатор ОТУ
                    ((kpframe *)st->userData)->Show();
                }

                // 5- сформировать и передать данные подключенным клиентам модуля Управление
                StationNetTS info(st);
                sendToAllClients(&info);

                // 6- если получены данные по актуальной станции - обновить панели информации по осн/рез блокам станции
                if (st == actualSt)
                {
                    ui->frame_mainBM->Show(st);
                    ui->frame_rsrvBM->Show(st);
                }

                st->IncSeanc();
            }
            break;

        // ошибка связи, param - актуальная станция
        case MSG_ERR:
            {
            Station * actualSt = (Station *)param;

            if (actualSt->GetSysInfo()->IsOnoff())
            {
                StationNetTS info((Station *)param);            // формируем уведомление
                sendToAllClients(&info);                        // передаем клиентам, если переход из рабочего состояния
            }
            //ui->statusBar->showMessage("Ошибка связи со станцией");
            }
            break;

        case MSG_ERR_TIMEOUT:                                   // ошибка тайм-аута
            //ui->statusBar->showMessage("Тайм-аут при приеме данных");
            break;

        case MSG_ERR_FORMAT:
            {
            Station * actualst = (Station *)param;              // гарантированно не nullptr
            //RasPacker* pack = (RasPacker* )param2;            // pack - весь пакет
            QString msg = QString("Ошибка формата при опросе ст.%1").arg(actualst->Name());
            Logger::LogStr(msg);
            ui->statusBar->showMessage(msg);
            }
            break;

        case MSG_ERR_CRC:                                       // ошибка CRC
            {
            //RasPacker* pack = (RasPacker* )param2;            // pack - весь пакет
            Station * actualst = (Station *)param;              // гарантированно не nullptr
            Logger::LogStr(QString("Ошибка CRC при опросе ст.%1").arg(actualst->Name()));
            }
            break;

        // передать сообщение
        case MSG_SND_NET:
            {
            RasPacker * pack = (RasPacker*)param;
            if (sndSocket)
                sndSocket->writeDatagram((char *)pack, pack->Length(), QHostAddress::Broadcast, portSnd);		// передача данных с указанием адреса(можно всем) и порта
            }
            GetMsg (MSG_SHOW_SND, param, param2);               // рекурсивный вызов для отображения
            break;

        default:
            break;
    }
    waterMsg.notify_all();
}


// глобальная функция отправки сообщения главному окну
// вызывается из рабочих потоков и работает в рабочих потоках
// генерирует сигнал MainWindow::SendMsg, QT обеспечивает обработку сигнала в основном потоке
// для гарантии целостности общих данных, после отправки сообщения ожидаем уведомления об обработке
// ожидание уведомления ограничено 1 сек, более чем достаточно для обработки и исключает зависание в случае ошибки в функции обработки
void SendMessage (int no, void * ptr, void * ptr2)
{
    emit MainWindow::mainWnd->SendMsg(no, ptr, ptr2);

    // ожидание обработки сообщения здесь гарантирует синхронизацию потоков с точки зрения целостности входного и выходного буферов
    // максимальное время ожидания здесь жестко - 1 сек, можно опционировать, хотя нет резона
    std::unique_lock<std::mutex> lck(sendMutex);
    waterMsg.wait_for(lck, std::chrono::milliseconds(1000));
}






//void Log (std::wstring s)
//{
//    Q_UNUSED (s)
//}

/*
// обработка сигнала-уведомления о готовности данных
void MainWindow::dataready(QByteArray data)
{
    ui->statusBar->showMessage(QString("Приняты данные: %1").arg(Logger::GetHex(data, 32)));
}

// обработка сигнала-уведомления об отсутствии данных в канала данных
void MainWindow::timeout()
{
    ui->statusBar->showMessage("Нет данных");
}

// обработка сигнала-уведомления об ошибке
void MainWindow::error  (int error)
{
    Q_UNUSED (error)
//    qDebug() << "Ошибка: " << BlockingRs::errorText(error);
}

// обработка сигнала-уведомления от старте потока RS
void MainWindow::rsStarted()
{
//    qDebug() << "Старт рабочего потока " << rasRs->name();
}
*/




