#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "hwswitch.h"
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
#include "threadpolling.h"

// Блок глобальных переменных проекта ==========================================================================================================================

const char * version = "1.0.1";                                 // версия приложения
const char * title = "ДЦ ЮГ на базе КП Круг. Станция связи";    // наименование приложения
const char * noperm = "Нет разрешений на ввод/вывод для управления переключением каналов";
const char * okperm = "Разрешения на ввод/вывод для коммутации каналов предоставлены";
const char * mainLineOn = "Попытка автопереключения коммутатора каналов на основную станцию связи";
const char * rsrvLineOn = "Попытка автопереключения коммутатора каналов на резервную станцию связи";

std::timed_mutex exit_lock;                                     // мьютекс, разрешающий завершение приложения
std::condition_variable waterMsg;                               // условная переменная для организации ожидания обработки сообщений
std::mutex sendMutex;                                           // мьютекс для доступа к waterMsg

MainWindow * MainWindow::mainWnd;                               // статический экземпляр главного окна

QString mainCom,                                                // порт прямого канала
        rsrvCom;                                                // порт обводного канала
QString configMain,                                             // строка конфигурации BlockingRS прямого канала
        configRsrv;                                             // строка конфигурации BlockingRS обратного канала
int     baud        = 57600;                                    // скорость обмена с модемами
int     delay       = 10;                                       // минимальная задержка между опросами, мс
int     breakdelay  = 50;                                       // максимально допустимый интервал между байтами в пакете, мс
quint64 driftCount  = 0;                                        // число корректных пакетов, принятых с чужого адреса

QString path;                                                   // путь к рабочему каталогу

bool activeRss = true;                                          // глобальный логический флаг активности РСС
bool activeRssPrv = true;                                       // глобальный логический флаг активности РСС в предыдущем такте

#ifdef Q_OS_WIN
QString editor = "notepad.exe";                                 // блокнот
#endif
#ifdef Q_OS_MAC
QString editor = "TextEdit";                                    // блокнот
#endif
#ifdef Q_OS_LINUX
QString editor = "gedit";                                       // блокнот, gedit, mousepad
#endif

QString images(":/status/images/");                             // путь к образам status/images
QString imagesEx(":/images/images/");                           // путь к образам images/images

Logger logger("log/mdmagent.log", true, true);                  // глобальный класс логгера

QString dbname      = "bd/arm.db";
QString esrdbbname  = "bd/arm.db";
QString extDb       = "bd/armext.db";
QString iniFile     = "mdmagent.ini";

// =============================================================================================================================================================



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadResources();                                            // загрузка графических ресурсов

    mainWnd = this;
    modulType=APP_MDMAGENT;                                     // тип приложения
    QString tmp;


    // --------------------------------------------------------------------------------------------------------------------------
    // блокируем мьютекс завершения работы
    exit_lock.lock();

    // инициализация переменныъ
    mainSql = rsrvSql = "";                                     // "DRIVER=QPSQL;Host=192.168.0.105;PORT=5432;DATABASE=blackbox;USER=postgres;PWD=358956";
    portSnd  = 0;                                               // порт передачи датаграмм
    portRcv  = 0;                                               // порт приема датаграмм
    netPulse = 0;                                               // частота в сек отправки квитанций для поддержки соединения
    mainRss = true;                                             // по умолчанию - основная РСС
    forcePassive = false;
    hardSwith = false;                                          // наличие аппаратного пкоммутатора
    hardSwitchAuto = true;                                      // автопереключение
    sndSocket = rcvSocket = sndFromMain = rcvFromMain = nullptr;
    msgMain = "ОСНОВН";                                         // сообщение от основной в резервную об активности основной
    msgRsrv = "РЕЗЕРВ";                                         // сообщение от основной в резервную об отключении основной (принудительное программное)
    activeRss = activeRssPrv = mainRss;                         // по умолчанию активной является основная РСС
    int ras = 1;                                                // по умолчанию RAS=1
    portTcp = 1002;                                             // по умолчанию порт 1002 - НЕ РАБОТАЕТ В DEBIAN
    tUcSnd = 0;                                                 // засечка передачи из УЦ
    timerAck = timerOR = timerAutoswitch = nullptr;
    dlgKp = nullptr;
    lastFromMain = QDateTime::currentDateTime();                // засечка времени
    nameMain = "Прямой";
    nameRsrv = "Обратный";

    // определяемся с файлом коныигурации: приоритет ini-файлу, заданному параметром командной строки (1-й параметр считается файлом конфигурации)
    QStringList list = QCoreApplication::arguments();
    if (list.count() > 1)
        iniFile = list[1];

    Logger::SetLoger(&logger);
    Logger::LogStr ("Конфигурация приложения: " + iniFile);

    path = QDir::currentPath();                                 // путь к текущему каталогу, преобразуем в путь к папке БД

    IniReader rdr(iniFile);
    rdr.GetText("DBNAME", dbname);                              // читаем опцию DBNAME, из имени БД получаем путь к папке БД
    QFileInfo fi(dbname);
    if (fi.isRelative())                                        // если задан относительный путь, получаем полный, добавляя к текущему каталогу
    {
        dbname = path + "/" + dbname;
        path += "/bd";
    }
    else
        path = QFileInfo(dbname).absoluteDir().absolutePath();  // если задан абсолютный путь, вычисляем корневой путь обратным ходом
    extDb       = path + "/armext.db";                          // путь к БД armext.db
    esrdbbname  = path + "/arm.db";                             // путь к БД ЕСР - по умолчанию берем изи arm.db

    rdr.GetText("ESRDBNAME"     , esrdbbname);                  // если опция ESRDBNAME задана явно, используем ее
    rdr.GetInt ("KRUG"          , ras   );                      // номер станции связи
    rdr.GetText("EDITOR"        , editor);                      // имя редактора
    rdr.GetInt ("TCPPORT"       , portTcp);                     // TCP-порт сервера входящих подключений модулей УПРАВЛЕНИЕ
    rdr.GetText("MAIN"          , mainCom);                     // порт прямого   канала
    rdr.GetText("RESERVE"       , rsrvCom);                     // порт обратного канала
    rdr.GetInt ("ВAUD"          , baud  );                      // скорость обмена с модемами
    rdr.GetInt ("DELAY"         , delay );                      // минимальная задержка между опросами станций
    rdr.GetInt ("READ_INTERVAL" , breakdelay);                  // максимально допустимый интервал между байтами в пакете, мс
    rdr.GetBool("WAITFORACK"    , g_rqAck);                     // ожидание квитанции
    rdr.GetText("SQLSERVERMAIN" , mainSql);                     // строка подключения к основному  SQL (postgresql)
    rdr.GetText("SQLSERVERRSRV" , rsrvSql);                     // строка подключения к резервному SQL (postgresql)
    rdr.GetBool("TRACESQL"      , SqlServer::logSql);           // лог SQL-запросов
    rdr.GetInt ("UDPSEND"       , portSnd);                     // порт передачи датаграмм
    rdr.GetInt ("UDPRECEIVE"    , portRcv);                     // порт приема датаграмм
    rdr.GetInt ("NETPULSE"      , netPulse  );                  // частота в сек отправки квитанций для поддержки соединения
    rdr.GetBool("MAINRSS"       , mainRss);                     // MAINRSS - ОN/OFF - основная/резервная
    rdr.GetBool("HARDWARESWITCH", hardSwith);                   // HARDWARESWITCH=OFF
    rdr.GetBool("AUTOSWITCH"    , hardSwitchAuto);              // AUTOSWITCH
    rdr.GetText("KANALMAIN"     , nameMain);                    // наименование прямого канала
    rdr.GetText("KANALRSRV"     , nameRsrv);                    // наименование обратного канала
    if (rdr.GetText("LINKRSS"   , tmp))                         // LINKRSS=192.168.0.101:7005
    {
        TcpHeader::ParseIpPort(tmp, nextRssIP, nextRssPort);
        Logger::LogStr(name());
    }
    // --------------------------------------------------------------------------------------------------------------------------

    // в имени окна - наименование модуля и тип РСС
    setWindowTitle(QString ("%1 %2").arg(title).arg(nextRssPort==0 ? "" : mainRss ? "(основная)" : "(резервная)"));

    if (!nameMain.length())        nameMain = mainCom;          // если не заданы имена каналов, использовать имена портов
    if (!nameRsrv.length())        nameRsrv = rsrvCom;
    ui->label_nameMain->setText(nameMain);                      // наименование прямого канала
    ui->label_nameRsrv->setText(nameRsrv);                      // наименование обратного канала

    KrugInfo * krug = nullptr;
//    Esr::ReadBd(esrdbbname, logger);                            // ЕСР
    Station::ReadBd(dbname, krug, logger, QString("WHERE RAS = %1 ORDER BY Addr").arg(ras));                      // станции

    blackbox = new SqlBlackBox(mainSql, rsrvSql, &logger);
    tmp = "Запуск";
    if (nextRssPort)
        tmp = QString("Запуск. %1. Смежная станция связи: %2:%3. Коммутатор: %4").arg(name()).arg(nextRssIP).arg(nextRssPort).arg(hardSwith ? "ДА" : "НЕТ");
    blackbox->putMsg(0, tmp, APP_MDMAGENT, LOG_NOTIFY);
    Logger::LogStr(tmp);



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

    // "выбираем" первую станцию
    if (Station::StationsOrg.size() > 0)
        SelectStation(Station::StationsOrg[0]);


    // создаем меню ПОМОЩЬ справа (используем setCornerWidget для отдельного QMenuBar)
    // и вложенные меню: О Программе, Протокол, О QT
    QMenuBar *bar = new QMenuBar(ui->menuBar);
    QMenu *menu = new QMenu("Помощь", bar);
    bar->addMenu(menu);

    QAction *action1 = new QAction("О программе", bar);
    menu->addAction(action1);
    connect(action1, SIGNAL(triggered()), this, SLOT(on_action_About_triggered()));

    QAction *action2 = new QAction("Протокол", bar);
    menu->addAction(action2);
    connect(action2, SIGNAL(triggered()), this, SLOT(action_load_log()));

    QAction *action3 = new QAction("О версии QT", bar);
    menu->addAction(action3);
    connect(action3, SIGNAL(triggered()), this, SLOT(on_action_QtAbout_triggered()));

    ui->menuBar->setCornerWidget(bar);


    // признаки основной/резервный для блоков отображения БМ
    ui->frame_mainBM->setRsrv(false);
    ui->frame_rsrvBM->setRsrv(true );

    // индикаторы прямого и обратного каналов
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::gray);
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::gray);

    // начальное состояние индикатора РСС О/Р - норма
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, Qt::darkGreen);
    ui->label_or->setPalette(pal);

    if (mainRss)
    {
        ui->label_main->setVisible(false);                      // в основной РСС скрываем индикатор О справа
        if (nextRssIP==0)                                       // если не определена связь между РСС, скрываем флажок отключения основной
            ui->checkBox_off->setVisible(false);
    }
    else
    {
        ui->checkBox_off->setVisible(hardSwith);                // в резервной РСС скрываем флажок отключения
        ui->label_or->setText("Р");                             // левую надпись О/Р устанавливаем в Р

        QPalette pal = palette();                               // цвет основной РСС изначально красный
        pal.setColor(QPalette::WindowText, Qt::red);
        ui->label_main->setPalette(pal);
    }

    // индикатор БПДК/УПОК: изначально серый
    // если принят пакет с данными ОТУ - моргнуть ярко-зеленым, обновить засечку времени
    // периодически (по таймеру) проверять на молчание более 1 мин: если засечки были, но молчит долго - желтый, иначе темно зеленый.
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::gray, Qt::darkGray);

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(int,void*,void*)), this, SLOT(GetMsg(int,void*,void*)));

    // синхронизация состояния флажка "С квитанцией" с переменной g_rqAck
    ui->checkBox_ack->setChecked(g_rqAck);

    // синхронизация состояния флажка "Полный опрос" с переменной Station::bFullPollingAll
    Station::bFullPollingAll = ui->checkBox_Full->isChecked();

    // конфигурация портов основного и обратного канала задаются в строках configMain, configRsrv
    if (mainCom.indexOf("OFF") < 0)
        configMain = QString("%1,%2,N,8,1").arg(mainCom).arg(baud);
    if (rsrvCom.indexOf("OFF") < 0)
        configRsrv = QString("%1,%2,N,8,1").arg(rsrvCom).arg(baud);

    // состояние флажков отключения основного и обратного каналов
    ui->checkBox_Main->setChecked(configMain.length() > 0);
    ui->checkBox_Main->setEnabled(/*configMain.length() > 0*/false);
    ui->checkBox_Rsrv->setChecked(configRsrv.length() > 0);
    ui->checkBox_Rsrv->setEnabled(/*configRsrv.length() > 0*/false);

    // запускаем рабочий поток опроса каналов
    pThreadPolling    = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling, (long)this));

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

    // запуск таймеров
    startTimer (1000);                                          // основной таймер MainWindow

    if (netPulse)                                               // если задана опция NETPULSE - запустить таймер timerAck
    {
        timerAck        = new QTimer(this);                     // таймер отпраки квитанций работоспособноси клиентам Управление (опция NETPULSE)
        connect(timerAck, SIGNAL(timeout()), this, SLOT(on_TimerAck()));
        timerAck ->start(netPulse * 1000);                      // время задано в сек
    }

    if (nextRssPort > 0)                                        // если задан порт связи основной и резервной РСС, стартуем таймер передачи timerOR
    {
        timerOR         = new QTimer(this);                     // таймер отправки сообщений о работоспособности основной РСС
        connect(timerOR, SIGNAL(timeout()) , this, SLOT(on_TimerOR()));
        timerOR->start(1000);

        if (mainRss)
        {
            sndFromMain = new QUdpSocket();                     // сокет для передачи датаграмм в резервную РСС
        }
        else
        {
            rcvFromMain = new QUdpSocket();                     // сокет для приема датаграмм в резервную РСС
            rcvFromMain->bind(QHostAddress::AnyIPv4, nextRssPort, QUdpSocket::ShareAddress);  // привязать сокет ко всем IP и порту
            connect(rcvFromMain, SIGNAL(readyRead()), this, SLOT(readFromMainRss()));
        }
    }

    // если есть коммутатор, запрашиваем права доступа, проверяем управляемость, и если все ОК - стартуем таймер автопереключения каналов
    if (hardSwith)
    {
        ui->label_switchStatus->set(QLed::ledShape::box, QLed::ledStatus::on, Qt::gray);
        ui->checkBox_off->setText(mainRss ? "Основ":"Резрв");
        if (!GetIoPermissin())
        {
            blackbox->putMsg(0, noperm, APP_MDMAGENT, LOG_ALARM);
            Logger::LogStr(noperm);
        }
        else
        {
            blackbox->putMsg(0, okperm,APP_MDMAGENT, LOG_NOTIFY);
            Logger::LogStr(QString("Проверка управления коммутатором: %1").arg(IsHwSwitchControlEnable() ? "ОК" : "ОШИБКА"));
            if (!hwSwitchControlEnable)
                blackbox->putMsg(0, "Ошибка проверки управления коммутатором",APP_MDMAGENT, LOG_ALARM);
        }

        // если все ОК, и опция AUTOSWITCH=ON, стартуем таймер автопереключения каналов
        if (hardSwitchAuto && IsHwSwitchControlEnable())
        {
            timerAutoswitch = new QTimer(this);                         // таймер отслеживания работоспособности аппаратуры и автопереключения РСС
            connect(timerAutoswitch, SIGNAL(timeout()), this, SLOT(on_TimerAutoswitch()));
            timerAutoswitch->start(5000);                               // каждые 5 сек
        }

        // если коммутатор не работает - флажок ОТКЛ блокируем
        if (!mainRss && !hwSwitchControlEnable/*(IsHwSwitchControlEnable())*/)
            ui->checkBox_off->setEnabled(false);
    }
    else
    {
        ui->label_switch->setVisible(false);
        ui->label_switchStatus->setVisible(false);
    }
}

// наименование: Основная/Резервная станция связи
QString MainWindow::name()
{
    return QString("%1 станция связи").arg(mainRss ? "Основная" : "Резервная");
}

MainWindow::~MainWindow()
{
    blackbox->putMsg(0, "Завершение работы", APP_MDMAGENT, LOG_NOTIFY);
    Logger::LogStr ("Деструктор MainWindow. Освобождаем ресурсы");

    exit_lock.unlock();

    if (blackbox != nullptr)
        delete blackbox;

    if (timerAck != nullptr)
        delete timerAck;
    if (timerOR != nullptr)
        delete timerOR;
    if (timerAutoswitch != nullptr)
        delete timerAutoswitch;
    if (sndFromMain)
        delete sndFromMain;
    if (rcvFromMain)
        delete rcvFromMain;
    if (sndSocket)
        delete sndSocket;
    if (rcvSocket)
        delete rcvSocket;

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
    ui->label_stName->setText(QString ("%1    Адрес: %2").arg(actualSt->Name(),-20).arg(actualSt->Addr()));
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
    else
    {
        waterMsg.notify_all();
    }
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
            blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
            blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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
        blackbox->putMsg(actualSt->No(), msg, APP_MDMAGENT, LOG_TECH);
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

    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, statusMain < 0 ? Qt::red : statusMain == 0 ? Qt::gray : statusMain==1 ? Qt::yellow : Qt::green);
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, statusRsrv < 0 ? Qt::red : statusRsrv == 0 ? Qt::gray : statusRsrv==1 ? Qt::yellow : Qt::green);
    if (hardSwith)
        ui->label_switchStatus->set(QLed::ledShape::box, QLed::ledStatus::on, !ioPermission ? Qt::red : hwSwitchControlEnable ? Qt::green : Qt::darkRed);
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


// О программе
void MainWindow::on_action_About_triggered()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );
    QMessageBox::about(this, "О программе", QString("%1\n%2\n\nФайл: %3.\nДата сборки: %4\n© ООО НПЦ Промавтоматика, 1992-2018").arg(title).arg(version).arg(info.filePath()).arg(info.lastModified().toString(FORMAT_DATETIME)));
}

void MainWindow::on_action_QtAbout_triggered()
{
    QMessageBox::aboutQt(this, "Версия QT");
}

QProcess process;
// протокол работы модуля
void MainWindow::action_load_log()
{
    QStringList params;
    params << logger.GetActualFile();
    process.start(editor, params);
    //process.waitForFinished(-1);
}




// ===============================================================================================================================================================
// слоты уведомлений сервера

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
            ((kpframe *)param)->SetActual(true, param2 != nullptr);
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




// =========================================================================================================================================================================
// переключение основная/резервная РСС и коммутация

// проверка подключения каналов коммутатора к станции связи
bool MainWindow::IsRssOnline()
{
    return mainRss ? IsMainRssSwitchOn() : !IsMainRssSwitchOn();
}


// станция активна?
bool MainWindow::IsActive()
{
    if (hardSwith)
    {
        // если коммутатор работает, анализ подключения, иначе - каждая станция считаеся активной и должна пытаться работать с каналами
        return hwSwitchControlEnable ? IsRssOnline() : true;
    }
    else
    {
        return mainRss ? !forcePassive : forcePassive || IsMainRssExpired();
    }
}


// активная РСС молчит более n сек
bool MainWindow::IsMainRssExpired()
{
    return !mainRss && lastFromMain.secsTo(QDateTime::currentDateTime()) > 3;
}


// клик по флажку отключения (переключения)
// при наличии коммутатора флажок доступен на обеих РСС, так как с обеих допустимо управление коммутатором
// при отсутствии коммутатора, флажок доступен ТОЛЬКО на основной (она в приоритете и ее можно логически отключить forcePassive = true)
void MainWindow::on_checkBox_off_clicked()
{
    QString msg;
    // при аппаратном управление переключать можно как с основной так и срезервной РСС
    // если есть коммутатор и он работает - работаем по коммутатору
    if (hardSwith && hwSwitchControlEnable /*IsHwSwitchControlEnable()*/)
    {
        msg = QString ("Принудительное переключение каналов на %1 станцию связи").arg(IsMainRssSwitchOn() ? "резервную" : "основную");
        if (QMessageBox::question(this, title, "Выполнить: " + msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            blackbox->putMsg(0, msg, APP_MDMAGENT, LOG_NOTIFY);
            Logger::LogStr(msg);
            if (mainRss)
                forcePassive = IsRssOnline();
            TurnSwitch();
            ShowStatusOP();
        }
        return;
    }

    ui->checkBox_off->setChecked(!forcePassive);

    // при программном управлении управление производится только с основной РСС
    msg = QString ("%1 опрос основной станции связи?").arg(forcePassive ? "Включить" : "Отключить");
    if (QMessageBox::question(this, title, msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
    {
        msg = QString("Команда принудительного %1 основной станции связи пользователем").arg(forcePassive ? "включения" : "отключения");
        forcePassive = !forcePassive;
        blackbox->putMsg(0, msg, APP_MDMAGENT, LOG_NOTIFY);
        Logger::LogStr(msg);
        ui->checkBox_off->setChecked(!forcePassive);
    }
}


// таймер отпраки квитанций работоспособноси клиентам Управление (опция NETPULSE)
void MainWindow::on_TimerAck()
{
    server->sentoAllAck();
}

// таймер отправки сообщений о работоспособности основно РСС
void MainWindow::on_TimerOR()
{
    // если основная РСС, отправляем статусное сообщение-датаграмму в резервную
    if (sndFromMain)
    {
        QByteArray msg = QTextCodec::codecForName("Windows-1251")->fromUnicode(forcePassive ? msgRsrv : msgMain);
        sndFromMain->writeDatagram(msg, QHostAddress(nextRssIP), nextRssPort);		// передача данных с указанием адреса(можно всем) и порта
    }
    ShowStatusOP();
}


// таймер отслеживания работоспособности аппаратуры и автопереключения РСС (1 раз в 5 сек)
// переключает каналы коммутатора в соответствии с логическим состоянием основной и резервной РСС
void MainWindow::on_TimerAutoswitch()
{
    // если основная, нет флага принудительного отключения и каналы не у нас
    if (mainRss && !forcePassive && !IsRssOnline())
    {
        Logger::LogStr(mainLineOn);
        blackbox->putMsg(0, mainLineOn, APP_MDMAGENT, LOG_NOTIFY);
        TurnSwitch();
    }

    if (!mainRss && (IsMainRssExpired() || forcePassive) && !IsRssOnline())
    {
        Logger::LogStr(rsrvLineOn);
        blackbox->putMsg(0,rsrvLineOn, APP_MDMAGENT, LOG_NOTIFY);
        TurnSwitch();
    }
}



// отобразить актуальное состояние элементов GUI и переходы О-Р
// вызывается из MainWindow::on_TimerOR() 1 раз в сек
// - большой индикатор О/Р нужным цветом в соответствии с состоянием
// - если резервная, цает основной РСС (справа) - в соответствии с состоянием
// - отслдить переходы состояния активная/пассивная
void MainWindow::ShowStatusOP()
{
    // цвет РСС в соответствии с состоянием
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, (activeRss = IsActive()) ? Qt::darkGreen : Qt::gray);
    ui->label_or->setPalette(pal);

    // если резервная, цает основной РСС (справа) - в соответствии с состоянием
    if (!mainRss)
    {
        QPalette pal = palette();
        pal.setColor(QPalette::WindowText, IsMainRssExpired() ? Qt::red : IsActive() ? Qt::white : Qt::darkGreen);
        ui->label_main->setPalette(pal);
    }

    // отслеживаем переходы состояния активная/пассивная для
    if (activeRss != activeRssPrv)
    {
        activeRssPrv = activeRss;
        QString msg = QString("%1 активности %2 станции связи").arg(activeRss ? "Включение" : "Отключение").arg(mainRss ? "основной" : "резервной");
        blackbox->putMsg(0, msg, APP_MDMAGENT, LOG_NOTIFY);
        Logger::LogStr(msg);
    }

    ui->checkBox_off->setChecked(IsActive());
}


// слот "прием датаграмм" ("ОСНОВН"/"РЕЗЕРВ") от основной РСС
void MainWindow::readFromMainRss()
{
    while (rcvFromMain->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(rcvFromMain->pendingDatagramSize());
        rcvFromMain->readDatagram(datagram.data(), datagram.size());
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        QString s = codec->toUnicode(datagram);

        // состояние forcePassive определяется содержимым датаграммы: если не "ОСНОВН", то forcePassive = true
        forcePassive = s.compare(msgMain) != 0;
        lastFromMain = QDateTime::currentDateTime();            // фиксируем время приема (наличие основной РСС )
    }
}




