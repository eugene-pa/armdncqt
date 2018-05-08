#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "../common/inireader.h"
#include "../spr/krug.h"
#include "../spr/esr.h"
#include "../spr/station.h"
#include "../spr/raspacker.h"
#include "../forms/dlgkpinfo.h"

// Прототипы функций рабочих потоков ПО КП
void   ThreadPolling		(long);							// функция потока опроса динии связи
std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения

MainWindow * MainWindow::mainWnd;                           // статический экземпляр главного окна

QString version = "1.0.1";                                  // версия приложения
QString title = "ДЦ ЮГ на базе КП Круг. Станция связи. ";

QString mainCom,                                            // порт прямого канала
        rsrvCom;                                            // порт обводного канала
QString configMain,                                         // строка конфигурации BlockingRS прямого канала
        configRsrv;                                         // строка конфигурации BlockingRS обратного канала
int     baud        = 57600;                                // скорость обмена с модемами
int     delay       = 10;                                   // минимальная задержка между опросами, мс
int     breakdelay  = 50;                                   // максимально допустимый интервал между байтами в пакете, мс

QString path;

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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWnd = this;
    modulType=APP_MDMAGENT;                                 // тип приложения
    QString tmp;

    // --------------------------------------------------------------------------------------------------------------------------
    path = QDir::currentPath();

    // если ini-файл задан параметром командной строки, используем
    QStringList list = QCoreApplication::arguments();
    if (list.count() > 1)
        iniFile = list[1];

    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

    IniReader rdr(iniFile);

    rdr.GetText("DBNAME", dbname);
    dbname = path + "/" + dbname;
    path = QFileInfo(dbname).absoluteDir().absolutePath();
    extDb       = path + "/bd/armext.db";
    esrdbbname  = path + "/bd/arm.db";


    int ras = 1;
    rdr.GetInt("KRUG"    , ras   );                             // номер станции связи
    portTcp = 1002;                                             // по умолчанию порт 1002
    rdr.GetInt("TCPPORT" , portTcp);                            // TCP-порт сервера входящих подключений модулей УПРАВЛЕНИЕ
    rdr.GetText("MAIN"   , mainCom);                            // порт прямого   канала
    rdr.GetText("RESERVE", rsrvCom);                            // порт обратного канала
    rdr.GetInt("DAUD"    , baud  );                             // скорость обмена с модемами
    rdr.GetInt("DELAY"   , delay );                             // минимальная задержка между опросами станций
    rdr.GetInt("READ_INTERVAL", breakdelay);                    // максимально допустимый интервал между байтами в пакете, мс

    // --------------------------------------------------------------------------------------------------------------------------

    KrugInfo * krug = nullptr;
//    Esr::ReadBd(esrdbbname, logger);                            // ЕСР
    Station::ReadBd(dbname, krug, logger, QString("WHERE RAS = %1 ORDER BY Addr").arg(ras));                      // станции

    loadResources();

    // формируем преставление станций в несколько строк
    // в качестве входного массива используем вектор станций StationsOrg, отсортированный по заданному при чтении БД критерию "Addr"
    int row = 0, col = 0, colmax = 13;
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
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(int,void*)), this, SLOT(GetMsg(int,void*)));

    // блокируем мьютекс завершения работы
    exit_lock.lock();

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

    QTableWidget * t = ui->tableWidget;
    t->setColumnCount(2);
    //t->setRowCount((int)Pereezd::Pereezds.size());
    //t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Клиент" << "Порт");
    t->resizeColumnsToContents();

    // автоматически растягтваем 1-й столбец
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // создание сервера TCP-подключений и привязка сигналов к слотам
    server = new ServerTcp(portTcp, QHostAddress("192.168.0.105"), &logger);
    QObject::connect(server, SIGNAL(acceptError  (ClientTcp*  )), this, SLOT(slotAcceptError      (ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*  )), this, SLOT(slotSvrNewConnection (ClientTcp*)));
    QObject::connect(server, SIGNAL(dataready    (ClientTcp*  )), this, SLOT(slotSvrDataready     (ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected (ClientTcp*  )), this, SLOT(slotSvrDisconnected  (ClientTcp*)));
    server->start();

}

MainWindow::~MainWindow()
{
    exit_lock.unlock();
    Station::Release();
    delete ui;
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


void MainWindow::closeEvent(QCloseEvent *)
{
/*
    if (rasRs)
    {
        emit (exit());
        if (rasRs != nullptr)
            delete rasRs;
        rasRs = nullptr;
    }
*/
}

void MainWindow::on_pushButtonMainOff_clicked()
{

}

void MainWindow::on_pushButtonRsrvOff_clicked()
{

}

void MainWindow::on_pushButtonToMain_clicked()
{

}

void MainWindow::on_pushButtonToRsrv_clicked()
{

}

void MainWindow::on_pushButtonTest_clicked()
{

}

void MainWindow::on_pushButtonATU_clicked()
{

}

void MainWindow::on_pushButtonReset_clicked()
{

}

void MainWindow::on_pushButtonGetReconnect_clicked()
{

}

void MainWindow::on_pushButtonResetMain_clicked()
{

}

void MainWindow::on_pushButtonResetRsrv_clicked()
{

}

void MainWindow::on_pushButtonWatchdog_clicked()
{

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


// обработка сообщений
void MainWindow::GetMsg (int np, void * param)
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
            ((kpframe *)param)->SetActual(true,false);
            break;

        case MSG_SHOW_SND:                                      // отобразить информацию о передаче запроса в КП
            {
            QString name = ((RasPacker*)param)->st == nullptr ? "?" : ((RasPacker*)param)->st->Name();
            ui->label_Snd->setText(QString("%1  -> %2").arg(Logger::GetHex(param, ((RasPacker*)param)->Length())).arg(name));
            }
            break;

        case MSG_SHOW_RCV:                                      // отобразить информацию об успешном приеме данных от КП
            if (param!=nullptr)
            {
                RasPacker* pack = (RasPacker* )param;           // pack - весь пакет
                Station * st = pack->st;                        //
                QString name = st == nullptr ? "?" : st->Name();//
                RasData * data = (RasData *)pack->data;         // data - блок данных
                st->GetRasData()->Copy(data);
                ui->label_RCV->setText(QString(" %1 :    %2...<- %3").arg(data->About()).arg(Logger::GetHex(param, std::min(48,((RasPacker*)param)->Length()))).arg(name));
                ui->statusBar->showMessage("Прием данных по ст. " + name);

                // обрабатываем системную информацию
                if (pack->Length() > 3 && data->LengthSys())
                {
                    // учесть переменную длину 9/15/30 и в случае 30 - записать оба блока
                    // первым всегда идет активный блок
                    bool act = pack->data[8] & 1;               // признак резерва
                    SysInfo * sysinfo = st->GetSysInfo(act);
                    sysinfo->Parse(data->PtrSys(), data->LengthSys());
                    if (data->LengthSys() == 30)
                    {
                        sysinfo = st->GetSysInfo(!act);
                        sysinfo->Parse(data->PtrSys()+15, 15);
                    }

                    // отрисовка актуального КП
                    ((kpframe *)st->userData)->Show();
                }

                // отобразить состояние актуальной станции
                if (st == actualSt)
                {
                    ui->frame_mainBM->Show(st);
                    ui->frame_rsrvBM->Show(st);
                }
            }
            break;

        case MSG_ERR_TIMEOUT:                                   // ошибка тайм-аута
            ui->statusBar->showMessage("Тайм-аут при приеме данных");
            break;

        case MSG_ERR_FORMAT:
            ui->statusBar->showMessage("Ошибка формата");
            break;

        case MSG_ERR_CRC:                                       // ошибка CRC
            Logger::LogStr ("Ошибка CRC");                      // лог
            break;

        default:
            break;
    }
    Q_UNUSED(param)
}

void Log (std::wstring s)
{
    Q_UNUSED (s)
}

// глобальная функция отправки сообщения главному окну
// вызывается из рабочих потоков и работает в рабочих потоках
// генерирует сигнал MainWindow::SendMsg, QT обеспечивает обработку сигнала в основном потоке
std::mutex sendMutex;
void SendMessage (int no, void * ptr)
{
    std::lock_guard <std::mutex> locker(sendMutex);
    emit MainWindow::mainWnd->SendMsg(no, ptr);
}


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

    //tooltip = true;
}


// уведомления сервера
// ошибка на сокете
void MainWindow::slotAcceptError(ClientTcp * conn)
{
    Q_UNUSED(conn);
}

// подключен новый клиент
void MainWindow::slotSvrNewConnection (ClientTcp *conn)
{
    QString s("Подключен клиент " + conn->name());
    //msg->setText(s);
}

void MainWindow::slotSvrDisconnected  (ClientTcp * conn)
{
    QString s("Отключен клиент " + conn->name());
    //msg->setText(s);
}

// получены данные
void MainWindow::slotSvrDataready     (ClientTcp * conn)
{
    QString name(conn->name());
    QString s("Приняты данные от клиента " + conn->name());
    //msg->setText(s);
}
