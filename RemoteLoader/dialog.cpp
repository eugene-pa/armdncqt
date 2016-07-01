#include "dialog.h"
#include "ui_dialog.h"

Logger logger("log/remoteloader.log", true, true);             // лог

QString log (QString msg)
{
    return logger.log(msg);
}

// порядок копирования:
// - установка соединения
// - запрос информации о сервере
// - запрос информации о файле/каталоге
// - если запрашиваем копирование файла - запрос временной копии, копирование файла
// - если запрашиваем копирование папки - запрос информации о файлах папки и поочередное копирование файлов
// - в случае отстутствия соединения более заданно времени сторожевого таймера breakT (по умолчанию 10 сек) - аврийный выход
// - в случае разрыва и повторной установки соединения до срабатывания сторожевого таймера - продолжаем копирование с файла, на
//   котором произошел разрыв

Dialog::Dialog(QStringList& list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{

    version = "RemoteLoader v.1.0.1";

    params = list;
    serverConnectStr = params[0];

    ui->setupUi(this);

    ui->labelAbout->setText(version);

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
    idTimer = startTimer(breakT);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_rejected()
{
    this->close();
    exit(-2);
}


// установлено соединение, начинаем процесс копирования
// ВАЖНО: при разрыве и повторной установке соединения процесс копирования начинается сначала
//        Такое поведение вытекает из реализации, но его можно считать вполне приемлемым, так как оно обеспечивает корректное копирование
void Dialog::connected   (ClientTcp * conn)
{
    killTimer(idTimer);                                     // останавливаем сторожевой таймер

    RemoteRq::localaddress  = conn->socket()->localAddress();
    RemoteRq::remoteaddress = conn->socket()->peerAddress();

    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::green);
    QString s("Установлено соединение с сервером " + conn->name());
    labelMsg->setText(s);
    log (s);

    // здесь можно проверить, не было ли прерывания процесса копирования, если было - продолжить с прерванного места
    if (_files.length())
    {
        log ("Продолжаем копирование файлов");
        info = _files[indx];
        localDstPath = params[2] + "/" + _files[indx]._name;    // назначение формируем из имени папки и файла
        RemoteRq rq(rqTempFile, serverConnectStr);
        rq.setParam(params[1] + "/" + _files[indx]._name);
        QByteArray data = rq.Serialize();
        connection->packsend(data);
    }
    else
    {
        done = 0;                                               // скопировано
        indx = 0;
        todo = 0;

        log ("Запрос информации о сервере");
        RemoteRq rq(rqAbout, serverConnectStr);
        QByteArray data = rq.Serialize();
        connection->packsend(data);
    }

}

// разорвано соединение
void Dialog::disconnected(ClientTcp * conn)
{
    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::yellow);
    QString s("Разорвано соединение с сервером " + conn->name());
    labelMsg->setText(s);
    log (s);

    // здесь нужно запустить таймер аварийного завершения, срабатывающий при отстутствии соединения в течение заданного времени, например, 10 сек
    idTimer = startTimer(breakT);
}


// ошибка сокета
void Dialog::error       (ClientTcp *conn)
{
    status->set(QLed::ledShape::round, QLed::ledStatus::on, Qt::yellow);
    QString s = QString("Ошибка подключения %1: %2").arg(conn->name()).arg(TcpHeader::ErrorInfo(conn->lasterror()));
    labelMsg->setText(s);
    log (s);
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
    QString s = QString("%1. Получен отклик от сервера %2: %3 байт").arg(QTime::currentTime().toString()).arg(conn->name()).arg(conn->rawLength());
    labelMsg->setText(s);
    //log(s);

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
        log (responce.toString());
        return;
    }

    switch (header.Rq())
    {

        case rqAbout:
        {
            // получили отклик о хосте; можно проверить совместимость версий
            ResponceAbout responce;
            responce.Deserialize(stream);
            log (responce.toString());
            //QMessageBox::information(this, "rqAbout", responce.toString());     // отобразим результат

            // запрашиваем инфу о файле
            log ("Запрашиваем информацию о файле " + params[1]);
            RemoteRq rq(rqFileInfo, serverConnectStr);
            rq.setParam(params[1]);
            QByteArray data = rq.Serialize();
            connection->packsend(data);

            break;
        }
        case rqFileInfo:
        {
            // получили информацию о файле
            ResponceFileInfo responce;
            responce.Deserialize(stream);

            if (responce.isDir())
            {
                log ("Запрашиваем информацию о файлах каталога " + params[1]);
                // удаленный объект копирования - каталог
                // - запрос списка файлов
                RemoteRq rq(rqFilesInfo, serverConnectStr);
                rq.setParam(params[1]);
                QByteArray data = rq.Serialize();
                connection->packsend(data);
            }
            else
            {
                info = responce.fileInfo();
                todo = info.length();                                   // общий размер файлов для копирования

                // копируем один файл: запрашиваем врменную копию
                localDstPath = params[2];                   // назначение задано явно

                log ("Запрашиваем создание временной копии файла " + params[1]);
                RemoteRq rq(rqTempFile, serverConnectStr);
                rq.setParam(params[1]);
                QByteArray data = rq.Serialize();
                connection->packsend(data);

            }
            // QMessageBox::information(this, "rqDirs", fileinfo.toString());
            break;
        }
        case rqFilesInfo:
        {
            // получили информацию о вложенных файлах папки
            ResponceFiles responce;
            responce.Deserialize(stream);
            _files = responce.files();                      // сохраняем список файлов папки для последующего копирования
            for (int i=0; i<_files.count(); i++)
                todo += _files[i].length();

            log ("Начинаем копирование файлов каталога " + params[1] + ":\n" + responce.toString());

            //QMessageBox::information(this, "rqFiles", files.toString());
            // стартуем копирование файлов из списка по одному
            indx = 0;
            info = _files[indx];
            localDstPath = params[2] + "/" + _files[indx]._name;    // назначение формируем из имени папки и файла
            RemoteRq rq(rqTempFile, serverConnectStr);
            rq.setParam(params[1] + "/" + _files[indx]._name);
            QByteArray data = rq.Serialize();
            connection->packsend(data);

            break;
        }
        case rqTempFile:
        {
            ResponceTempFile responce;
            responce.Deserialize(stream);
            if (responce.exist())
            {
                log (QString("Создана временная копия файла %1: %2").arg(responce.nameSrc(),responce.name()));
                // создана временная копия нужного файла, начинаем копирование
                // QMessageBox::information(this, "rqTempFile", temp.toString());

                ui->labelFrom->setText(QString("Откуда: %1/%2").arg(params[0], responce.nameSrc()));
                ui->labelTo  ->setText(QString("Куда: %1"     ).arg(localDstPath));

                rqReadFile(responce.name(), localDstPath, 0, blocksize);
            }
            else
                QMessageBox::information(this, "rqTempFile", "Запрошенный файл отсутствует на диске");
            break;
        }

        case rqRead:
        {
            ResponceRead responce;
            responce.Deserialize(stream);
            labelMsg->setText(responce.toString());            //QMessageBox::information(this, "rqRead", read.toString());

            // можно прямо тут писать без сигнала
            emit(ReadNext(responce));
            break;
        }

    }

}


// чтение файла
void Dialog::slotReadNext(ResponceRead&  responce)
{
    QFile file(responce.dstfilepath());

    done += responce.length();
    int value = (int)(((qreal)done/todo)*100);
    ui->progressBarAll->setValue(value);

    value = (int)(100*((qreal)(responce.offset() + responce.length())/info.length()));
    ui->progressBar->setValue(value);

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
        ui->progressBar->setValue(100);
        //QMessageBox::information(this, "rqRead", "Файл скопирован!");
        log ("Файл скопирован!");

        // изменить дату и время модификации файла консольной командой "touch -t YYYYMMDDHHMM.SS"
        // эта команда работает в Windows 10
        QProcess process(this);
        QString cmd = QString("touch -t %1 '%2'").arg(info._lastChanged.toString("yyyyMMddhhmm.ss")).arg(responce.dstfilepath());
        process.start(cmd);
        process.waitForFinished();

        // можно здесь удалить временную копию; а можно попытаться возложить эту функцию на сервер

        if (++indx >= _files.count())
        {
            ui->progressBarAll->setValue(100);
            log ("Копирование завершено, завершение работы");
            close();
            exit(0);
        }
        else
        {
            // переходим к следующему файлу
            info = _files[indx];
            localDstPath = params[2] + "/" + _files[indx]._name;    // назначение формируем из имени папки и файла
            RemoteRq rq(rqTempFile, serverConnectStr);
            rq.setParam(params[1] + "/" + _files[indx]._name);
            QByteArray data = rq.Serialize();
            connection->packsend(data);
        }
    }
}

// формирование запроса на чтение фрагмента файда
void Dialog::rqReadFile(QString src, QString dst, qint64 offset, int length)
{
    if (offset==0)
        log (QString("Старт копирования %1  -->  %2").arg(src).arg(dst));

    RemoteRq rq(rqRead,serverConnectStr);

    //ui->labelFrom->setText(QString("Откуда: %1/%2").arg(params[0], src));
    //ui->labelTo  ->setText(QString("Куда: %1"     ).arg(dst));


    rq.setParam(src);                                       // файл
    rq.setParam2(offset);                                   // смещение
    rq.setParam3(length);                                   // длина запрашиваемого блока данных
    rq.setParam4(dst);                                      // назначение (куда копируем)

    QByteArray data = rq.Serialize();
    connection->packsend(data);

}


// аварийный таймер
void Dialog::timerEvent(QTimerEvent *event)
{
    log (QString("Аварийное завершение - нет соедиения с сервером %1 ").arg(params[0]));
    close();
    exit(-1);
}
