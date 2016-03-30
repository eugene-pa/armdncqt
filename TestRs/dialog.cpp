#include "dialog.h"
#include "ui_dialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
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

// обработка уведомления об изменении параметров порта
void Dialog::applysettings()
{
    ui->label->setText(settingdlg->description());
}


// соединить
void Dialog::on_pushButton_Open_clicked()
{
    if (ui->pushButton_Open->isChecked())
    {
        rs = new BlockingRs(this);
        COMMTIMEOUTS tm = { 5000,0, 5000, 1, 500 };
        rs->startRs(settingdlg->description(), tm);

/*
//        serial = new RsBase(settingdlg->description());
//        if (serial->isOpen())
        {
            worker = new MdmAgentReader();
            worker->moveToThread(&readerThread);
            connect(this, &Dialog::operate, worker, &MdmAgentReader::readData);
            readerThread.start();
            emit (operate(settingdlg->description()));  //serial

//            QByteArray array("12345-");
//            int n = serial->send(array);
    //        serial->write(array);
        }
//        else
//        {
//            QMessageBox::critical(this, tr("Ошибка ") + serial->name(), serial->errorString());
//        }
*/
    }
//    else
//    if (serial != nullptr)
//    {
//        serial->close();
//    }
}


// реализация пользовательского протокола с таймированием
int MdmAgentReader::readData(QString settings/*class RsBase* serial*/)
{
    serial = new RsBase(settings);
    if (serial->isOpen())
    {
        QByteArray array("12345-");
        int n = serial->send(array);

        char ch1 = serial->GetChar(10000);
        char ch2 = serial->GetChar(10000);
        char ch3 = serial->GetChar(10000);
    }
    return 0;
}


