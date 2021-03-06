#include "mainwindow.h"
#include "ui_mainwindow.h"

Logger logger("Log/paserver.log", true, true);             // лог

QString iniFile = "paServer.ini";                           // настройки

#ifdef Q_OS_WIN
//QString iniFile = "c:/armdncqt/paServer/paServer.ini";    // настройки
QString images(":/status/images/");                         // путь к образам
QString editor = "notepad.exe";                             // блокнот
#endif
#ifdef Q_OS_MAC
//QString iniFile = "/Users/evgenyshmelev/armdncqt/paServer/paServer.ini";  // настройки
QString images("/Users/evgenyshmelev/armdncqt/images/");    // путь к образам
QString editor = "TextEdit";                                // блокнот
#endif
#ifdef Q_OS_LINUX
//QString iniFile = "/home/eugene/QTProjects/paServer/paServer.ini";      // настройки
QString images("../images/");
QString editor = "gedit";                                   // блокнот
#endif

int port = 28080;


QString log(QString msg)                                       // глобальная функция лога
{
    return logger.log(msg);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    version = "paServer v.1.0.1";

    ui->setupUi(this);

    loadResources();
    logger.log("Запуск");

    // порт, на котором работает сервис, по умолчанию = 208080
    IniReader rdr(iniFile);
    rdr.GetInt("PORT", port);

    ui->statusBar->addPermanentWidget(portInfo = new QLabel("Порт: " + QString::number(port)), true);   //
    portInfo->setMaximumWidth(75);

    ui->statusBar->addPermanentWidget(serverStatus = new QLed(ui->statusBar, QLed::round, QLed::on, Qt::yellow));
    serverStatus->setFixedSize(15,15);

    ui->statusBar->addPermanentWidget(msg = new QLabel(), true);   //

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

    idTimerConnections = startTimer(timerTrashConnectionsInterval); // каждую секунду очищаем корзину соединений
    idTimerTempFiles   = startTimer(delayTrashTempFiles*1000);      // каждый час очищаем корзину временных файлов
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

    serverStatus->set(QLed::round, QLed::on, server->clients().count() > 0 ? Qt::green : Qt::yellow);
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

    serverStatus->set(QLed::round, QLed::on, server->clients().count() > 0 ? Qt::green : Qt::yellow);
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

    rq->setsrc(conn->socket()->peerAddress());
    rq->setdst(conn->socket()->localAddress());

    s = QString("Обработка запроса %1, src=%2, dst=%3").arg(RemoteRq::getRqName(rq->Rq())).arg(rq->getsrc().toString()).arg(rq->getdst().toString());
    if (rq->Rq() != rqRead)
        logger.log(s);
    msg->setText(s);

    // Пример: 127.0.0.1:28080/192.168.0.1:28080
    if (rq->isRemote())
    {
        QString ipport = rq->getRemotePath(), host, path;

        if (RemoteRq::ParseNestedIp(ipport, host, path))
        {
            // здесь надо обработать ошибку установки связи: сокет не должен повисать навечно, нужно определить некоторое время, после чего
            // остановить сокет и поместить в корзину;
            // можно вернуть отрицательную квитанцию
            //
            logger.log(s = QString("Перенаправляем запрос %1 хосту %2").arg(RemoteRq::getRqName(rq->Rq())).arg(host));
            rq->setRemote(path);                            // изымаем свой адрес из пути
            ClientTcp * client = new ClientTcp(host, &logger, true, remoteClientId);
            conn->setUserPtr(0,client);                     // в оригинальном сокете запоминаем потомка
            client->setUserPtr(0,rq);                       // в сокете-потомке запоминаем запрос
            client->setUserPtr(1,conn);                     // в сокете-потомке запоминаем родителя

            QObject::connect(client, SIGNAL(connected   (ClientTcp*)), this, SLOT(nextConnected   (ClientTcp*)));
            QObject::connect(client, SIGNAL(disconnected(ClientTcp*)), this, SLOT(nextdisconnected(ClientTcp*)));
            QObject::connect(client, SIGNAL(error       (ClientTcp*)), this, SLOT(nexterror       (ClientTcp*)));
            QObject::connect(client, SIGNAL(dataready   (ClientTcp*)), this, SLOT(nextdataready   (ClientTcp*)));
            client->start();
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
                QByteArray data = responce.Serialize();
                conn->packsend(data);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                break;
            }
            case rqDirs:
            {
                ResponceDirs responce(*rq, &logger);
                QByteArray data = responce.Serialize();
                conn->packsend(data);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                break;
            }
            case rqFileInfo:
            {
                ResponceFileInfo responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                QByteArray data = responce.Serialize();
                conn->packsend(data);
                break;
            }
            case rqFilesInfo:
            {
                ResponceFiles responce(*rq, &logger);
                ui->tableWidget->item(row,2)->setText(responce.toString());
                QByteArray data = responce.Serialize();
                conn->packsend(data);
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
                QByteArray data = responce.Serialize();
                conn->packsend(data);
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
                QByteArray data = responce.Serialize();
                conn->packsend(data);
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
                // ВАЖНО:
                // rq->getsrc() - источник   запроса, но место назначения копирования
                // rq->getdst() - получатель запроса(ответчик), но источник копирования
                if (responce.offset()==0)
                    logger.log(CString("Старт копирования файла %1 --> %2. откуда=%3, куда=%4").arg(responce.srcfilepath()).arg(responce.dstfilepath()).arg(rq->getdst().toString()).arg(rq->getsrc().toString()));
                ui->tableWidget->item(row,2)->setText(responce.toString());
                QByteArray data = responce.Serialize();
                conn->packsend(data);
                break;
            }
            default:
                break;
        }
    }
}

// ошибка на сокете
void MainWindow::slotAcceptError(ClientTcp * conn)
{
    Q_UNUSED(conn);
}

// настроечный файл
void MainWindow::on_actionIni_triggered()
{
    QStringList params;
    params << iniFile;
    processIni.start(editor, params);
}

// протокол работы (лог)
void MainWindow::on_actionLog_triggered()
{
    QStringList params;
    params << logger.GetActualFile();
    processLog.start(editor, params);
}

void MainWindow::on_actionAbout_triggered()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );
    QMessageBox::about(this, "О программе", QString("ДЦ ЮГ. Сервер удаленного доступа paServer\n%1\n\nФайл: %2.\nДата сборки: %3\nВерсия протокола: %4.0\n\n© ООО НПЦ Промавтоматика, 2016").arg(version).arg(info.filePath()).arg(info.created().toString(FORMAT_DATETIME)).arg(RemoteRq::paServerVersion));
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
void MainWindow::nextConnected   (ClientTcp* conn)
{
    QString s("Подключен клиент " + conn->name() + ". Транслируем запрос");
    msg->setText(s);

    RemoteRq * rq = (RemoteRq *)conn->userPtr(0);
    QByteArray data = rq->Serialize();
    conn->packsend(data);
}

void MainWindow::nextdisconnected(ClientTcp* conn)
{
    RemoteRq * rq     = (RemoteRq *)conn->userPtr(0);
    ClientTcp* parent = (ClientTcp*)conn->userPtr(1);
    ResponceError responce(*rq, Disconnect, &logger);
    QByteArray data = responce.Serialize();
    parent->packsend(data);

    conn->stop();
    // сохраняем отработанные сокеты в корзине
    _trash.append(conn);                                    // удаление экземпляра тут приводит к проблемам: delete conn;

}

void MainWindow::nexterror       (ClientTcp* conn)
{
    RemoteRq * rq     = (RemoteRq *)conn->userPtr(0);
    ClientTcp* parent = (ClientTcp*)conn->userPtr(1);
    ResponceError responce(*rq, Timeout, &logger);
    responce.setErrorText( conn->socket()->errorString());
    QByteArray data = responce.Serialize();
    parent->packsend(data);

    conn->stop();
    // сохраняем отработанные сокеты в корзине
    _trash.append(conn);                                    // удаление экземпляра тут приводит к проблемам: delete conn;

}

// приняли данные в сокете-потомке (рекурсивное подключение)
// ретранслируем данные в сокет родителя
void MainWindow::nextdataready   (ClientTcp* conn)
{
    ClientTcp* parent = (ClientTcp*)conn->userPtr(1);
    parent->send(conn->rawData(), conn->rawLength());
    parent->setUserPtr(0,nullptr);                          // в оригинальном сокете удаляем потомка

    // можно сразу остановить сокет и разорвать соединение
    // непонятно с удалением...

    conn->stop();
    // сохраняем отработанные сокеты в корзине
    _trash.append(conn);                                    // удаление экземпляра тут приводит к проблемам: delete conn;
}


// пришлось встроить корзину и отдельный код для удаления классов ClientTcp
// удаляю объекты и выбираю их из корзины по таймеру
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (_trash.length() > 0)
    {
        delete _trash[0];
        _trash.removeAt(0);
    }

    // каждый час очищаем корзину временных файлов
    if (event->timerId() == idTimerTempFiles)
    {
        while (ResponceTempFile::trashTempFiles.length() > 0)
        {
            QFileInfo info(ResponceTempFile::trashTempFiles[0]);
            if ((int)(QDateTime::currentDateTime().toTime_t() - info.lastRead().toTime_t()) > delayTrashTempFiles)
            {
                msg->setText(log("Удаляем временный файл " + info.filePath()));
                QFile(info.filePath()).remove();
                ResponceTempFile::trashTempFiles.removeAt(0);
            }
            else
                break;                                      // оставшиеся файлы - "свежие"
        }
    }
}
