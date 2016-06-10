#include "mainwindow.h"
#include "ui_mainwindow.h"

Logger logger("Log/paserver.log", true, true);             // лог

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

    ui->statusBar->addPermanentWidget(msg = new QLabel(), true);   //
    loadResources();
    logger.log("Запуск");

    // порт, на котором работает сервис, по умолчанию = 208080
    IniReader rdr(iniFile);
    rdr.GetInt("PORT", port);

    QTableWidget * t = ui->tableWidget;
    t->setColumnCount(3);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setColumnWidth(0,90);
    t->setColumnWidth(1,300);
    t->setColumnWidth(2,300);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "Клиент" << "Запрос" << "Отклик");
    t->setSortingEnabled(false);
    t->setRowCount(0);

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

    QTableWidget * t = ui->tableWidget;
    t->setRowCount(server->clients().count());
    int row = t->rowCount()-1;
    QString name = conn->name();
    t->setItem(row,0, new QTableWidgetItem (conn->name()));
    t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)conn));    // запомним клиента
    t->setItem(row,1, new QTableWidgetItem (""));
    t->setItem(row,2, new QTableWidgetItem (""));
}

void MainWindow::slotSvrDisconnected  (ClientTcp * conn)
{
    QString s("Отключен клиент " + conn->name());
    msg->setText(s);

    int row = findRowByConn(conn);
    if (row>=0)
    {
        ui->tableWidget->removeRow(row);
    }
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

    RemoteRq * rq = new RemoteRq();                         // нельзя создавать на стеке
    rq->Deserialize(stream);

    logger.log(s = QString("Обработка запроса %1, src=%2, dst=%3").arg(RemoteRq::getRqName(rq->Rq())).arg(rq->getsrc().toString()).arg(rq->getdst().toString()));
    msg->setText(s);

    if (rq->isRemote())
    {
        QString host, path;
        if (RemoteRq::ParseNestedIp(rq->getRemotePath(), host, path))
        {
            rq->setRemote(path);                             // изымаем свой адрес из пути
            ClientTcp * client = new ClientTcp(host, &logger, true, remoteClientId);
            conn->setUserPtr(client);
            client->setUserPtr(&rq);                        //

            QObject::connect(client, SIGNAL(connected   (ClientTcp*)), this, SLOT(nextConnected   (ClientTcp*)));
            QObject::connect(client, SIGNAL(disconnected(ClientTcp*)), this, SLOT(nextdisconnected(ClientTcp*)));
            QObject::connect(client, SIGNAL(error       (ClientTcp*)), this, SLOT(nexterror       (ClientTcp*)));
            QObject::connect(client, SIGNAL(dataready   (ClientTcp*)), this, SLOT(nextdataready   (ClientTcp*)));
        }
        else
        {
            logger.log(s = QString("Ошибка разбора IP адреса/порта запроса: %1").arg(rq->getRemotePath()));
            msg->setText(s);
        }
    }
    else
    {
        int row = findRowByConn(conn);
        if (row>=0)
        {
            ui->tableWidget->item(row,1)->setText(rq->toString());
        }
        // разбор типа запроса
        switch (rq->Rq())
        {
            case rqAbout:
            {
                ResponceAbout responce(*rq, &logger);
                conn->packsend(responce.Serialize());
                ui->tableWidget->item(row,2)->setText(responce.toString());
                break;
            }
            case rqDirs:
            {
                ResponceDirs responce(*rq, &logger);
                conn->packsend(responce.Serialize());
                ui->tableWidget->item(row,2)->setText(responce.toString());
                break;
            }
            case rqFileInfo:
            {
                ResponceFileInfo responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                conn->packsend(responce.Serialize());
                break;
            }
            case rqFilesInfo:
            {
                ResponceFiles responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                conn->packsend(responce.Serialize());
                break;
            }
            case rqFilesSize:
            {
                break;
            }
            case rqDrives:
            {
                ResponceDrives responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                conn->packsend(responce.Serialize());
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
                ResponceTempFile responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                conn->packsend(responce.Serialize());
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
                ResponceRead responce(*rq);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                conn->packsend(responce.Serialize(), true);
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

void MainWindow::on_actionIni_triggered()
{

}

void MainWindow::on_actionLog_triggered()
{

}

void MainWindow::on_actionAbout_triggered()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );
    QMessageBox::about(this, "О программе", QString("ДЦ ЮГ. Сервер удаленного доступа paServer\n\nФайл: %1.\nДата сборки: %2\nВерсия протокола: %3.0\n\n© ООО НПЦ Промавтоматика, 2016").arg(info.filePath()).arg(info.created().toString(FORMAT_DATETIME)).arg(RemoteRq::paServerVersion));
}

void MainWindow::on_actionQT_about_triggered()
{
    QMessageBox::aboutQt(this, "Версия QT");
}

int MainWindow::findRowByConn(ClientTcp *conn)
{
    for (int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem * item = ui->tableWidget->item(i,0);
        if (item != nullptr)
        {
            if (conn == (ClientTcp *) ui->tableWidget->item(i,0)->data(Qt::UserRole).value<void*>())
                return i;
        }
    }
    return -1;
}


// уведомления клиентов рекурсивных подключений
void MainWindow::nextConnected   (ClientTcp*)
{

}

void MainWindow::nextdisconnected(ClientTcp*)
{

}

void MainWindow::nexterror       (ClientTcp*)
{

}

void MainWindow::nextdataready   (ClientTcp*)
{

}

