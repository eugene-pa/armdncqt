#include <QRegExpValidator>
#include <QTextCodec>
#include <QTime>
#include "../common/clienttcp.h"
#include "../common/logger.h"
#include "dialog.h"
#include "ui_dialog.h"


Logger logger("Log/shaper.txt", true, true);



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // Валидатор с помощью регулярного выражения
    ;
    ui->lineEdit_IP->setValidator(new QRegExpValidator(QRegExp("\\b([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}\\b"),this));
    //ui->lineEditToSend->setValidator(new QRegExpValidator(QRegExp("[0-9a-f A-F ]+"),this));
}

Dialog::~Dialog()
{
    delete ui;
}

// привязка слота выполняется в контекстном меню контрола GOTO SLOT (перейти к слоту)
// судф по всему, явный вызов coonect вфполняется в функции setupUi - >QMetaObject::connectSlotsByName(Dialog), вызываемой в метафайле UI_Dialog.h
void Dialog::on_pushButtonStart_clicked()
{
    QString ipport = ui->lineEdit_IP->text();
    ui->label_msg->setText(ipport);

    client = new ClientTcp(ipport, &logger);
    client->setid("АРМ ШН");
    QObject::connect(client, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(client, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(client, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(client, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(client, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    client->start();

    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    ui->lineEditToSend->setEnabled(true);
}

void Dialog::on_pushButtonStop_clicked()
{
    client->stop();
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    ui->lineEditToSend->setEnabled(false);
}


// установлено соединение
void Dialog::connected   (ClientTcp *client)
{
    ui->label_msg->setText(msg = QString("Соединение c хостом %1").arg(client->Name()));
    logger.log(msg);
}

// разорвано соединение
void Dialog::disconnected(ClientTcp *client)
{
    ui->label_msg->setText(msg = QString("%1. Разрыв соединения c клиентом %2").arg(QTime::currentTime().toString()).arg(client->Name()));
    logger.log(msg);
}

// ошибка сокета
void Dialog::error (ClientTcp *client)
{

}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void Dialog::dataready   (ClientTcp * client)
{
    ui->label_msg->setText(msg = QString("%1. Получены форматные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()));
    ui->labelRcv->setText(Logger::GetHex(client->RawData(), qMin(client->RawLength(),16)));
    logger.log(msg);
    client->SendAck();
}

// получены необрамленные данные - отдельный сигнал
void Dialog::rawdataready(ClientTcp *client)
{
    ui->label_msg->setText(msg = QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(client->RawLength()));
    ui->labelRcv->setText(Logger::GetHex(client->RawData(), qMin(client->RawLength(),16)));
    logger.log(msg);

}



void Dialog::on_pushButtonSend_clicked()
{
    client->Send(QTextCodec::codecForName("Windows-1251")->fromUnicode(ui->lineEditToSend->text()));
}
