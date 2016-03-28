#include "dialog.h"
#include "ui_dialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include "settingsdialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    settingdlg = new SettingsDialog(this);
    connect(settingdlg, SIGNAL(applysettings()),this, SLOT(applysettings()));
    settingdlg->show();
}

void Dialog::on_pushButton_Open_clicked()
{
    if (ui->pushButton_Open->isChecked())
    {
        serial = new QSerialPort(settingdlg->settings().name);
    }
}

void Dialog::applysettings()
{
    ui->label->setText(settingdlg->description());
}
