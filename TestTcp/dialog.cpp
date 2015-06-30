#include "dialog.h"
#include "ui_dialog.h"
#include <QRegExpValidator>
#include <QTextCodec>
#include <QTime>

QTcpSocket * client;
QString msg;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Валидатор с помощью регулярного выражения
    QRegExp reg("\\b([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}\\b");
    ui->lineEdit_IP->setValidator(new QRegExpValidator(reg,this));

    QObject::connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(on_pushStart()));


}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_pushStart()
{
    QString ipport = ui->lineEdit_IP->text();
    ui->label_msg->setText(ipport);

    client = new QTcpSocket(this);
    QObject::connect(client, SIGNAL(connected()), this, SLOT(slotConnected()));
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    QObject::connect(client, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
    QObject::connect(client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));

    client->connectToHost("127.0.0.1",1010);
}

void Dialog::slotReadyRead()
{
    ui->label_msg->setText(msg = "Готов к чтению");

    char data[65535];
    int length = client->read(data, sizeof(data));

    ui->label_msg->setText(QString("%1. Получены данные: %2 байт").arg(QTime::currentTime().toString()).arg(length));
}

void Dialog::slotConnected()
{
    ui->label_msg->setText(msg = "Соединение");

    //QByteArray("АРМ ШН");
    QByteArray id = QTextCodec::codecForName("Windows-1251")->fromUnicode(QByteArray("АРМ ШН"));
    id[id.length()] = 0;

    client->write(id);
}

void Dialog::slotError (QAbstractSocket::SocketError er)
{
    ui->label_msg->setText(msg = "Ошибка" + er);
    ui->label_msg->setText(QString("%1. Ошибка: %2").arg(QTime::currentTime().toString()).arg(TcpHeader::ErrorInfo(er)));
    client->connectToHost("127.0.0.1",1010);
}

void Dialog::slotDisconnected()
{
    ui->label_msg->setText(QString("%1. Разрыв соединения").arg(QTime::currentTime().toString()));
}

