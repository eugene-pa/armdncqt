#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"


Logger logger("Log/bridgetcp.log", true, true);             // лог

#ifdef Q_OS_WIN
QString iniFile = "c:/armdncqt/bridgetcp/bridgetcp.ini";    // настройки
QString images(":/status/images/");                         // путь к образам
QString editor = "notepad.exe";                             // блокнот
#endif
#ifdef Q_OS_MAC
QString iniFile = "/Users/evgenyshmelev/armdncqt/bridgetcp/bridgetcp.ini";  // настройки
QString images("/Users/evgenyshmelev/armdncqt/images/");    // путь к образам
QString editor = "TextEdit";                                // блокнот
#endif
#ifdef Q_OS_LINUX
QString iniFile = "/home/eugene/QTProjects//bridgetcp/bridgetcp.ini";       // настройки
QString images("../images/");
QString editor = "gedit";                                   // блокнот
#endif

bool compressEnabled = true;                                // сжатие на летy при передаче

QString     serverConnectStr;                               // ip:порт основного сервера

void log (QString& msg)
{
    logger.log(msg);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(msg = new QLabel());   //

    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::off, Qt::green, Qt::yellow);
}

MainWindow::~MainWindow()
{
    delete ui;
}


// установлено соединение
void MainWindow::connected   (ClientTcp * conn)
{
    RemoteRq::localaddress  = conn->socket()->localAddress();
    RemoteRq::remoteaddress = conn->socket()->peerAddress();
    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::on);

    ui->actionAbout->setEnabled(true);
    ui->actionDirs->setEnabled(true);
    ui->actionFiles->setEnabled(true);
    ui->actionFileUnfo->setEnabled(true);
    ui->actionRead->setEnabled(true);
    ui->actionTempCopy->setEnabled(true);

    QString s("Установлено соединение с сервером " + conn->name());
    msg->setText(s);
}

// разорвано соединение
void MainWindow::disconnected(ClientTcp * conn)
{
    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::off, Qt::green, Qt::yellow);

    ui->actionAbout   ->setEnabled(false);
    ui->actionDirs    ->setEnabled(false);
    ui->actionFiles   ->setEnabled(false);
    ui->actionFileUnfo->setEnabled(false);
    ui->actionRead    ->setEnabled(false);
    ui->actionTempCopy->setEnabled(false);

    QString s("Разорвано соединение с сервером " + conn->name());
    msg->setText(s);
}

// ошибка сокета
void MainWindow::error       (ClientTcp *conn)
{
    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::off, Qt::green, Qt::yellow);
    QString s = QString("Ошибка подключения %1: %2").arg(conn->name()).arg(TcpHeader::ErrorInfo(conn->lasterror()));
    msg->setText(s);
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void MainWindow::dataready   (ClientTcp * conn)
{
    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::on);
    msg->setText(QString("%1. Получен отклик от сервера %2: %3 байт").arg(QTime::currentTime().toString()).arg(conn->name()).arg(conn->rawLength()));

    // обработка отклика
    QBuffer buf;
    buf.setData(conn->data(), conn->length());
    buf.open(QIODevice::ReadOnly);
    QDataStream stream(&buf);

    HeaderResponce header;
    header.Deserialize(stream);
    switch (header.Rq())
    {
        case rqAbout:
        {
            ResponceAbout responce;
            responce.Deserialize(stream);
            QMessageBox::information(this, "rqAbout", responce.toString());     // отобразим результат

            break;
        }
        case rqDirs:
        {
            ResponceDirs responce;
            responce.Deserialize(stream);
            QMessageBox::information(this, "rqDirs", responce.toString());
            break;
        }
        case rqFileInfo:
        {
            ResponceFileInfo fileinfo;
            fileinfo.Deserialize(stream);
            QMessageBox::information(this, "rqDirs", fileinfo.toString());
            break;
        }
        case rqFilesInfo:
        {
            ResponceFiles files;
            files.Deserialize(stream);
            QMessageBox::information(this, "rqFiles", files.toString());
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
            ResponceTempFile temp;
            temp.Deserialize(stream);
            QMessageBox::information(this, "rqTempFile", temp.toString());
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
            break;
        }
        default:
            break;
    }


}

// получены необрамленные данные - отдельный сигнал
void MainWindow::rawdataready(ClientTcp * conn)
{
    ui->labelStatus->set(QLed::ledShape::round, QLed::ledStatus::on);
    msg->setText(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(conn->rawLength()));
}


void MainWindow::on_pushButton_clicked()
{
    serverConnectStr = ui->lineEdit->text();
    connection = new ClientTcp(serverConnectStr, &logger, compressEnabled, "remoteClient");

    QObject::connect(connection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(connection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(connection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(connection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(connection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));

    connection->start();
}

// запрос "О хосте и сервисе"
void MainWindow::on_actionAbout_triggered()
{
    RemoteRq rq(rqAbout);
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}

// запрос списка каталогов заданной папки
void MainWindow::on_actionDirs_triggered()
{
    RemoteRq rq(rqDirs);
    rq.setParam(ui->lineEditFolder->text());
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}

// запрос пакета информации о файлах заданной папки
void MainWindow::on_actionFiles_triggered()
{
    RemoteRq rq(rqFilesInfo);
    rq.setParam(ui->lineEditFolder->text());
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}

// запрос информации о файле
void MainWindow::on_actionFileUnfo_triggered()
{
    RemoteRq rq(rqFileInfo);
    rq.setParam(ui->lineEditFolder->text());
    QByteArray data = rq.Serialize();
    connection->packsend(data);

}

// запрос чтения заданного файла
void MainWindow::on_actionRead_triggered()
{
    RemoteRq rq(rqRead);
    rq.setParam(ui->lineEditFolder->text());                // файл
    rq.setParam2(0);                                        // смещение
    rq.setParam3(1024);                                     // длина запрашиваемого блока данных
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}

void MainWindow::on_actionTempCopy_triggered()
{
    RemoteRq rq(rqTempFile);
    rq.setParam(ui->lineEditFolder->text());
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}
