#include "mainwindow.h"
#include "ui_mainwindow.h"

Logger logger("Log/bridgetcp.log", true, true);             // лог

#ifdef Q_OS_WIN
QString iniFile = "c:/armdncqt/paServer/paServer.ini";    // настройки
QString images(":/status/images/");                         // путь к образам
QString editor = "notepad.exe";                             // блокнот
#endif
#ifdef Q_OS_MAC
QString iniFile = "/Users/evgenyshmelev/armdncqt/paServer/paServer.ini.ini";  // настройки
QString images("/Users/evgenyshmelev/armdncqt/images/");    // путь к образам
QString editor = "TextEdit";                                // блокнот
#endif
#ifdef Q_OS_LINUX
QString iniFile = "/home/eugene/QTProjects/paServer/paServer.ini.ini";  // настройки
QString images("../images/");
QString editor = "gedit";                                   // блокнот
#endif

int port = 28080;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    ui->statusBar->addPermanentWidget(msg = new QLabel());   //
    loadResources();
    logger.log("Запуск");

    // порт, на котором работает сервис, по умолчанию = 208080
    IniReader rdr(iniFile);
    rdr.GetInt("PORT", port);

    server = new ServerTcp(port, QHostAddress::Any, &logger);
    QObject::connect(server, SIGNAL(acceptError(ClientTcp*)), this, SLOT(slotAcceptError(ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*)), this, SLOT(slotSvrNewConnection(ClientTcp*)));
    QObject::connect(server, SIGNAL(dataready(ClientTcp*)), this, SLOT(slotSvrDataready(ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected(ClientTcp*)), this, SLOT(slotSvrDisconnected(ClientTcp*)));

    server->start();
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
    g_white             = new QPixmap(images + "icon_non.ico");
}


// уведомления сервера
// подключен новый клиент
void MainWindow::slotSvrNewConnection (ClientTcp *conn)
{
    QString s("Подключен клиент " + conn->name());
    msg->setText(s);

//    QTableWidget * t = ui->tableWidget;
//    t->setRowCount(connections.count() + server->clients().count());
//    int row = t->rowCount()-1;
//    QString name = conn->name();
//    t->setItem(row,0, new QTableWidgetItem (conn->name()));
//    t->setItem(row,1, new QTableWidgetItem (conn->getid()));
//    t->setItem(row,2, new QTableWidgetItem (QString("%1/%2").arg(conn->getsent(0)).arg(conn->getsent(1))));
//    t->setItem(row,3, new QTableWidgetItem (QString("%1/%2").arg(conn->getrcvd(0)).arg(conn->getrcvd(1))));
//    t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)conn));    // запомним клиента
}

void MainWindow::slotSvrDisconnected  (ClientTcp * conn)
{
    QString s("Отключен клиент " + conn->name());
    msg->setText(s);
}

// получены данные
void MainWindow::slotSvrDataready     (ClientTcp * conn)
{
    QString name(conn->name());
    QString s("Приняты данные от клиента " + conn->name());
    msg->setText(s);
}

// ошибка на сокете
void MainWindow::slotAcceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
}
