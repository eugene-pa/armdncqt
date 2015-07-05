#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"

Logger logger("Log/shaper.txt", true, true);
QVector<ShapeSet *> sets;                                           // массив форм
ColorScheme * colorScheme;

QString server_ipport = "192.168.0.100:1013";                       // подключение к потоку ТС из настроечного файла

#ifdef Q_OS_WIN
    QString dbname("C:/armdncqt/bd/arm.db");
    QString extDb ("C:/armdncqt/bd/armext.db");
    QString form  ("C:/armdncqt/pictures/Назаровский.shp");         // Табло1
    QString images("../images/");                                   // путь к образам
#endif
#ifdef Q_OS_MAC
    QString dbname("/Users/evgenyshmelev/armdncqt/bd/arm.db");
    QString extDb ("/Users/evgenyshmelev/armdncqt/bd/armext.db");
    QString form  ("/Users/evgenyshmelev/armdncqt/Pictures/Назаровский.shp");
    QString images("/Users/evgenyshmelev/armdncqt/images/");       // путь к образам
#endif
#ifdef Q_OS_LINUX
    QString dbname("/home/eugene/QTProjects/armdncqt/bd/arm.db");
    QString extDb ("/home/eugene/QTProjects/armdncqt/bd/armext.db");
    QString form  ("/home/eugene/QTProjects/armdncqt/pictures/Назаровский.shp");
    QString images("../images/");                                   // путь к образам
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Logger::SetLoger(&logger);
    Logger::LogStr ("Запуск приложения");

    ui->setupUi(this);

    mdiArea = new QMdiArea;                                             // создаем виджет MDI
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);       // скролбары
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    // загрузка графики
    green   = new QPixmap(images + "icon_grn.ico");
    red     = new QPixmap(images + "icon_red.ico");
    yellow  = new QPixmap(images + "icon_yel.ico");

    // добавляем в статус бар поля "IP_ПОРТ" и индикатор соединения
    ui->statusBar->addPermanentWidget(new QLabel(server_ipport));   //
    hostStatus.setPixmap(*yellow);
    ui->statusBar->addPermanentWidget(&hostStatus);

    // загрузка НСИ
    Esr::ReadBd(dbname, logger);
    Station::ReadBd(dbname, logger);
    IdentityType::ReadBd (extDb, logger);
    Ts::ReadBd (dbname, logger);
    Tu::ReadBd (dbname, logger);

    colorScheme = new ColorScheme(extDb, &logger);
    TrnspDescription::readBd(extDb, logger);

    // инициализация сетевых клиентов
    clientTcp = new ClientTcp(server_ipport, &logger, false, "АРМ ШН");
    QObject::connect(clientTcp, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(clientTcp, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    clientTcp->start();

    //QString s ("D:/APO/ArmDnc11/2013.08.22. RAS/@ras1_15.arh");
    //ArhReader a(s);
    //a.Next();
    //a.Next();
    //a.Next();
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
    ShapeChild * child = new ShapeChild(new ShapeSet(form, &logger));
    mdiArea->addSubWindow(child);
    child->show();
    //ShapeChild
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Варилка.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Васюринская.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Гетмановская.shp"));
}



// обработка уведомлений ClientTcp
// установлено соединение
void MainWindow::connected   (ClientTcp *client)
{
    hostStatus.setPixmap(*green);
    ui->statusBar->showMessage(QString("Соединение c хостом %1").arg(client->Name()), 60000);
}

// разорвано соединение
void MainWindow::disconnected(ClientTcp *client)
{
    hostStatus.setPixmap(*yellow);
    ui->statusBar->showMessage(QString("%1. Разрыв соединения c клиентом %2").arg(QTime::currentTime().toString()).arg(client->Name()), 60000);
}

// ошибка сокета
void MainWindow::error (ClientTcp *client)
{
    hostStatus.setPixmap(*red);
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void MainWindow::dataready   (ClientTcp * client)
{
    ui->statusBar->showMessage(QString("%1. Получены форматные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()), 10000);
    client->SendAck();
}

// получены необрамленные данные - отдельный сигнал
void MainWindow::rawdataready(ClientTcp *client)
{
    ui->statusBar->showMessage(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()), 10000);
}
