#include "dialog.h"
#include "ui_dialog.h"
//#include <../common/rsproxy.h>
#include <../common/rsasinc.h>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //rs = new RsProxy("COM3,9600,N,8,1");
    rs = new RsAsinc("COM3,9600,N,8,1");

    rs->SetTimeWaiting(10);
    startTimer(50);
}

Dialog::~Dialog()
{

    delete ui;
}

void Dialog::on_pushButton_clicked()
{

}

// обработка событий таймера
void Dialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    try
    {
        while (true)
        {
            instr += (char)rs->GetChEx();
        }
    }
    catch (RsException e)
    {
        // нет данных; игнорируем
    }
    ui->textEdit->setText(instr);

}

void Dialog::on_pushButton_2_clicked()
{
    QByteArray data = ui->lineEdit->text().toUtf8();
    rs->Send(data);
}
