#include <QRegExpValidator>
#include <QTextCodec>
#include <QTime>
#include "../common/tcpheader.h"
#include "../common/logger.h"
#include "dialog.h"
#include "ui_dialog.h"


QTcpSocket * client;
QString msg;
QString ip;
int port;
Logger logger("Log/shaper.txt", true, true);

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Валидатор с помощью регулярного выражения
    QRegExp reg("\\b([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}\\b");
    ui->lineEdit_IP->setValidator(new QRegExpValidator(reg,this));
}

Dialog::~Dialog()
{
    delete ui;
}


char _data[65535];
int _length = 0;
int _toRead = 4;

void Dialog::slotReadyRead()
{
    ui->label_msg->setText(msg = "Готов к чтению");
    logger.log(msg);

    while (true)
    {
        _length += client->read(_data+_length, _toRead - _length);
        if (_length < _toRead)
            return;
        if (_length==4)
        {
            if (((TcpHeader *)_data)->Signatured())
            {
                qDebug() << "Сигнатура";
                _toRead += ((TcpHeader *)_data)->Length();
            }
            else
            {
                qDebug() << "Неформатные данные";
                return;
            }
        }
        else
            break;

    }

    ui->label_msg->setText(msg = QString("%1. Получены данные: %2 байт").arg(QTime::currentTime().toString()).arg(_length));
    ui->labelRcv->setText(Logger::GetHex(_data, qMin(_length,16)));
    logger.log(msg);
}

void Dialog::slotConnected()
{
    ui->label_msg->setText(msg = "Соединение");
    logger.log(msg);

    //QByteArray("АРМ ШН");
    QByteArray id = QTextCodec::codecForName("Windows-1251")->fromUnicode(QByteArray("АРМ ШН"));
    id[id.length()] = 0;

    client->write(id);
}

void Dialog::slotError (QAbstractSocket::SocketError er)
{
    ui->label_msg->setText(msg = QString("%1. Ошибка: %2").arg(QTime::currentTime().toString()).arg(TcpHeader::ErrorInfo(er)));
    logger.log(msg);
    if (client->state() != QAbstractSocket::ConnectedState)
        client->connectToHost(ip,port);
}

void Dialog::slotDisconnected()
{
    ui->label_msg->setText(msg = QString("%1. Разрыв соединения").arg(QTime::currentTime().toString()));
    logger.log(msg);
    client->connectToHost(ip,port);
}


// привязка слота выполняется в контекстном меню контрола GOTO SLOT (перейти к слоту)
// судф по всему, явный вызов coonect вфполняется в функции setupUi - >QMetaObject::connectSlotsByName(Dialog), вызываемой в метафайле UI_Dialog.h
void Dialog::on_pushButtonStart_clicked()
{
    QString ipport = ui->lineEdit_IP->text();
    TcpHeader::ParseIpPort(ipport, ip, port);

    ui->label_msg->setText(ipport);

    client = new QTcpSocket(this);
    QObject::connect(client, SIGNAL(connected()), this, SLOT(slotConnected()));
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    QObject::connect(client, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
    QObject::connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));

    client->connectToHost(ip,port);
}

