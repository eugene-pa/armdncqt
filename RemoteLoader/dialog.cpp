#include "dialog.h"
#include "ui_dialog.h"

Logger logger("log/remoteloader.log", true, true);             // лог

void log (QString msg)
{
    logger.log(msg);
}

Dialog::Dialog(QStringList& list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    params = list;
    ui->setupUi(this);

    // добавляю статус бар
    this->layout()->addWidget(statusBar = new QStatusBar());
    labelMsg = new QLabel(QString("Подключение к удаленному хосту %1 ...").arg(list[0]));
    statusBar->addWidget(labelMsg, 1);
    labelMsg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    status = new QLed(statusBar,QLed::ledShape::round, QLed::ledStatus::on, Qt::yellow);
    status->setFixedSize(15,15);
    statusBar->addPermanentWidget(status);

    ui->labelFrom->setText(QString("Откуда: %1/%2").arg(list[0], list[1]));
    ui->labelTo  ->setText(QString("Куда: %1"     ).arg(list[2]));

    connection = new ClientTcp(list[0], &logger, true, "");

    QObject::connect(connection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(connection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(connection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(connection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
//    QObject::connect(connection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));

    QObject::connect(this, SIGNAL(ReadNext (ResponceRead&)), this, SLOT(slotReadNext(ResponceRead&)));

    connection->start();

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_rejected()
{
    this->close();
}

// установлено соединение
void Dialog::connected   (ClientTcp * conn)
{
    RemoteRq::localaddress  = conn->socket()->localAddress();
    RemoteRq::remoteaddress = conn->socket()->peerAddress();

    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::green);
    QString s("Установлено соединение с сервером " + conn->name());
    labelMsg->setText(s);

    RemoteRq rq(rqAbout, params[0]);
    QByteArray data = rq.Serialize();
    connection->packsend(data);
}

// разорвано соединение
void Dialog::disconnected(ClientTcp * conn)
{
    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::yellow);
    QString s("Разорвано соединение с сервером " + conn->name());
    labelMsg->setText(s);
}


// ошибка сокета
void Dialog::error       (ClientTcp *conn)
{
    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::yellow);
    QString s = QString("Ошибка подключения %1: %2").arg(conn->name()).arg(TcpHeader::ErrorInfo(conn->lasterror()));
    labelMsg->setText(s);
}


// получены необрамленные данные - отдельный сигнал
//void Dialog::rawdataready(ClientTcp *)
//{
//    // игнорируем
//}


// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void Dialog::dataready   (ClientTcp * conn)
{
    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::green);
    labelMsg->setText(QString("%1. Получен отклик от сервера %2: %3 байт").arg(QTime::currentTime().toString()).arg(conn->name()).arg(conn->rawLength()));

    // обработка отклика
    QBuffer buf;
    buf.setData(conn->data(), conn->length());
    buf.open(QIODevice::ReadOnly);
    QDataStream stream(&buf);

    HeaderResponce header;
    header.Deserialize(stream);
    header.setsrc(conn->socket()->peerAddress());           // IP адреса src/dst берем из соединения
    header.setdst(conn->socket()->localAddress());

    // если ошибка - разбор ошибки
    if (header.error())
    {
        ResponceError responce;
        responce.Deserialize(stream);
        QMessageBox::information(this, "rqError", responce.toString());     // отобразим результат
        return;
    }

    switch (header.Rq())
    {
        case rqAbout:
        {
            ResponceAbout responce;
            responce.Deserialize(stream);
            QMessageBox::information(this, "rqAbout", responce.toString());     // отобразим результат
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
        case rqTempFile:
        {
            ResponceTempFile temp;
            temp.Deserialize(stream);
            if (temp.exist())
                QMessageBox::information(this, "rqTempFile", temp.toString());
            else
                QMessageBox::information(this, "rqTempFile", "Запрошенный файл отсутствует на диске");
            break;
        }

        case rqRead:
        {
            ResponceRead read;
            read.Deserialize(stream);
            labelMsg->setText(read.toString());            //QMessageBox::information(this, "rqRead", read.toString());

            // можно прямо тут писать без сигнала
            emit(ReadNext(read));
            break;
        }

    }

}


// чтение файла
void Dialog::slotReadNext(ResponceRead&  responce)
{
    QFile file(responce.dstfilepath());
    if (file.open(QIODevice::ReadWrite))
    {
        bool ret = file.seek(responce.offset());
        file.write(responce.data());
        file.close();
    }

    if (!responce.isEof())
    {
        // рекурсивный вызов запроса на чтение
        rqReadFile(responce.srcfilepath(), responce.dstfilepath(), responce.offset() + responce.length(), blocksize);
    }
    else
    {
        QMessageBox::information(this, "rqRead", "Файл скопирован!");
    }
}

// формирование запроса на чтение фрагмента файда
void Dialog::rqReadFile(QString src, QString dst, qint64 offset, int length)
{
    RemoteRq rq(rqRead,params[0]);
    rq.setParam(src);                                       // файл
    rq.setParam2(offset);                                   // смещение
    rq.setParam3(length);                                   // длина запрашиваемого блока данных
    rq.setParam4(dst);                                      // назначение (куда копируем)

    QByteArray data = rq.Serialize();
    connection->packsend(data);

}
