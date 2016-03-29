#include "dialog.h"
#include "ui_dialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include "settingsdialog.h"
#include "rsbase.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    settingdlg = nullptr;
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


// вызов диалога пармаметров порта
void Dialog::on_pushButton_clicked()
{
    if (settingdlg==nullptr)
    {
        settingdlg = new SettingsDialog(this);
        connect(settingdlg, SIGNAL(applysettings()),this, SLOT(applysettings()));
        settingdlg->show();
    }
    else
        settingdlg->setVisible(true);
}


// соединить
void Dialog::on_pushButton_Open_clicked()
{
    if (ui->pushButton_Open->isChecked())
    {
        serial = new RsBase(settingdlg->description());

        worker = new MdmAgentReader();
        worker->moveToThread(&readerThread);
        connect(this, &Dialog::operate, worker, &MdmAgentReader::readData);
        readerThread.start();
        emit (operate(serial));
    }
}


// реализация пользовательского протокола с таймированием
int MdmAgentReader::readData(class RsBase* serial)
{
    char ch1 = serial->GetChar(10000);
    char ch2 = serial->GetChar(1000);
    char ch3 = serial->GetChar(1000);
    return 0;
}


void Dialog::applysettings()
{
    ui->label->setText(settingdlg->description());
}

