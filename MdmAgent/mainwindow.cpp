#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "../common/inireader.h"
#include "../spr/krug.h"
#include "../spr/esr.h"
#include "../spr/station.h"
#include "../spr/raspacker.h"

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
int     port        = 1002;                                 // TCP-порт сервера входящих подключений модулей УПРАВЛЕНИЕ

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
    rdr.GetInt("TCPPORT" , port  );                             // TCP-порт сервера входящих подключений модулей УПРАВЛЕНИЕ
    rdr.GetText("MAIN"   , mainCom);                            // порт прямого   канала
    rdr.GetText("RESERVE", rsrvCom);                            // порт обратного канала
    rdr.GetInt("DAUD"    , baud  );                             // скорость обмена с модемами
    rdr.GetInt("DELAY"   , delay );                             // минимальная задержка между опросами станций
    rdr.GetInt("READ_INTERVAL", breakdelay);                    // максимально допустимый интервал между байтами в пакете, мс

    // --------------------------------------------------------------------------------------------------------------------------

    KrugInfo * krug = nullptr;
//    Esr::ReadBd(esrdbbname, logger);                            // ЕСР
    Station::ReadBd(dbname, krug, logger, QString("WHERE RAS = %1 ORDER BY Addr").arg(ras));                      // станции

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
}

MainWindow::~MainWindow()
{
    exit_lock.unlock();
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
                ui->label_RCV->setText(QString("[ %1 ]:    %2...<- %3").arg(data->About()).arg(Logger::GetHex(param, std::min(48,((RasPacker*)param)->Length()))).arg(name));
                ui->statusBar->showMessage("Прием данных по ст. " + name);

                // обрабатываем системную информацию
                if (pack->Length() > 3 && data->LengthSys())
                {
                    // учесть переменную длину 9/15/30 и в случае 30 - записать оба блока
                    SysInfo * sysinfo = st->GetSysInfo(pack->data[8] & 1);
                    sysinfo->Parse(data->PtrSys(), data->LengthSys());

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
