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

void log(QString& msg)                                      // глобальная функция лога
{
    logger.log(msg);
}

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

// получен запрос
// если запрос локальный - выясняем тип и обрабатываем
// если запрос удаленный - создаем клиента и привязываем обработчики;
//                         - при подключении - транслировать запрос
//                         - при приеме - транслировать ответ заказчику и отключиться
void MainWindow::slotSvrDataready     (ClientTcp * conn)
{
    QString name(conn->name());
    QString s("Принят запрос от клиента " + conn->name());
    msg->setText(s);

    QBuffer buf;
    buf.setData(conn->data(), conn->length());
    buf.open(QIODevice::ReadOnly);
    QDataStream stream(&buf);

    RemoteRq rq;
    rq.Deserialize(stream);
    if (rq.isRemote())
    {
        // переадресация дальше по сети
        int a = 99;
    }
    else
    {
        // разбор типа запроса
        switch (rq.Rq())
        {
            case rqAbout:
            {
                ResponceAbout responce(rq);
                conn->packsend(responce.Serialize());
                break;
            }
            case rqDirs:
            {
                ResponceDirs responce(rq);
                conn->packsend(responce.Serialize());
                break;
            }
            case rqFileInfo:
            {
                ResponceFileInfo fileinfo(rq);
                conn->packsend(fileinfo.Serialize());
                break;
            }
            case rqFilesInfo:
            {
                ResponceFiles files(rq);
                conn->packsend(files.Serialize());
                break;
            }
            case rqFilesSize:
            {
                break;
            }
            case rqDrives:
            {
                break;
            }
            case rqProcesses:
            {
                break;
            }
            case rqProcesseInfo:
            {
                break;
            }
            case rqTempFile:
            {
                ResponceTempFile temp(rq);
                conn->packsend(temp.Serialize());
                break;
            }
            case rqTempFilesZip:
            {
                break;
            }
            case rqTempDirZip:
            {
                break;
            }
            case rqDeleteTemp:
            {
                break;
            }
            case rqRead:
            {
                ResponceRead read(rq);
                QByteArray data = read.Serialize();
                int length = data.length();
                conn->packsend(data, true);
                break;
            }
            default:
                break;
        }
    }
}

// ошибка на сокете
void MainWindow::slotAcceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
}
