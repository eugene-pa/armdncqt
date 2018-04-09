#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "../common/inireader.h"
#include "../spr/krug.h"
#include "../spr/esr.h"
#include "../spr/station.h"

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
        ui->gridLayout_kp->addWidget(new kpframe(this, st),row,col++);
        if (col==colmax)
        {
            col = 0;
            row++;
        }
    }
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(int,void*)), this, SLOT(GetMsg(int,void*)));

    exit_lock.lock();
    configMain = QString("%1,%2,N,8,1").arg(mainCom).arg(baud);
    pThreadPolling    = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling, (long)&configMain));
}

MainWindow::~MainWindow()
{
    exit_lock.unlock();
    delete ui;
}


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


// обработка сообщений
void MainWindow::GetMsg (int np, void * param)
{
    switch (np)
    {
        case 0:
            {
            Logger::LogStr (*(QString *)param);                                                  // лог
            //ui->statusBar->showMessage(QString::fromStdWString(pMsg->GetText()));   // GUI - строка состояния окна
            }
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
