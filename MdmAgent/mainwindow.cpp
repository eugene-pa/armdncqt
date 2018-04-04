#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"
#include "../common/inireader.h"

// Прототипы функций рабочих потоков ПО КП
void   ThreadPolling		(long);							// функция потока опроса динии связи
std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения

MainWindow * MainWindow::mainWnd;                           // статический экземпляр главного окна

QString version = "1.0.1";                                  // версия приложения
QString title = "ДЦ ЮГ на базе КП Круг. Станция связи. ";

#ifdef Q_OS_WIN
    QString path = "C:/armdncqt/";
    QString editor = "notepad.exe";                         // блокнот
#endif
#ifdef Q_OS_MAC
    QString path = "/Users/evgenyshmelev/armdncqt/";
    QString editor = "TextEdit";                             // блокнот
#endif
#ifdef Q_OS_LINUX
    QString path = "/home/dc/armdncqt/";
    QString editor = "gedit";                               // блокнот
#endif
    Logger logger(path + "/log/mdmagent.log", true, true);

    QString images(":/status/images/");                     // путь к образам status/images
    QString imagesEx(":/images/images/");                   // путь к образам images/images

    QString dbname;
    QString esrdbbname;
    QString extDb;

    QString iniFile =       "mdmagent.ini";



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWnd = this;
    modulType=APP_MDMAGENT;                                 // тип приложения

    // если ini-файл задан параметром командной строки, используем
    QStringList list = QCoreApplication::arguments();
    if (list.count() > 1)
        iniFile = list[1];
    // если задан конфигурационный файл, читаем настройки и подстраиваем пути
    // pathQDir::currentPath();                             // текущий каталог - по умолчанию
    // iniFile = "armtoola.ini";                            // так будем брать настройки из тек.каталога, если ini-файл не задан в параметрах
    IniReader rdr(iniFile);
    if (rdr.GetText("WORKINDIRECTORY", path))               // рабочая папка
        logger.ChangeActualFile(path + "log/armtools.log");

    dbname      = path + "/bd/arm.db";
    esrdbbname  = path + "/bd/arm.db";
    extDb       = path + "/bd/armext.db";

    QString tmp;

    // добавляем фреймы для каждой станции
    ui->gridLayout_kp->addWidget(new kpframe(this),0,0);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,1);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,2);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,3);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,4);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,5);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,6);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,7);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,8);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,9);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,10);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,11);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,12);

    ui->gridLayout_kp->addWidget(new kpframe(this),1,0);
    ui->gridLayout_kp->addWidget(new kpframe(this),1,1);
    ui->gridLayout_kp->addWidget(new kpframe(this),1,2);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,3);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,4);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,5);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,6);

    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::yellow);

    //rasRs = nullptr;
    config = L"COM1,38400,N,8,1";
    pThreadPolling    = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling   , (long)&config));

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(WORD,void*)), this, SLOT(GetMsg(WORD,void*)));

}

MainWindow::~MainWindow()
{
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
void MainWindow::GetMsg (WORD np, void * param)
{
    switch (np)
    {
        case 0:
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
void SendMessage (WORD no, void * ptr)
{
    std::lock_guard <std::mutex> locker(sendMutex);
    emit MainWindow::mainWnd->SendMsg(no, ptr);
}
