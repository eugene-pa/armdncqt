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
    rs = nullptr;
    settingdlg = nullptr;
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    qDebug() << "~Dialog()";
    delete ui;
    qDebug() << "end ~Dialog()";
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

// слот-обработчик уведомлений о готовности данных
void Dialog::dataready(QByteArray data)
{
    if (data.length())
        ui->textEdit->setPlainText(ui->textEdit->toPlainText() + /*QString(data)*/GetHex(data,32) + "\n");
}

// слот-обработчик уведомлений о неготовности данных
void Dialog::timeout()
{
    ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "Нет данных...\n");
}

void Dialog::error  (int error)
{

    ui->textEdit->setPlainText(ui->textEdit->toPlainText() + BlockingRs::errorText((BlockingRs::DataError)error) + ": " + rs->errorText());
}

// соединить
void Dialog::on_pushButton_Open_clicked()
{
    if (ui->pushButton_Open->isChecked())
    {
        rs = new BlockingRs(this, 1, 1024);
        connect(rs, SIGNAL(dataready(QByteArray)), this, SLOT(dataready(QByteArray)));
        connect(rs, SIGNAL(timeout()), this, SLOT(timeout()));
        connect(rs, SIGNAL(error(int)), this, SLOT(error(int)));
        connect(this, SIGNAL(exit()), rs, SLOT(exit()));

        connect(rs, SIGNAL(finished()), this, SLOT(rsFinished));
        connect(rs, SIGNAL(started() ), this, SLOT(rsStarted()));

        // определяем задержки и стартуем поток
        COMMTIMEOUTS tm = { 100, 1, 3000, 1, 250 };
        //rs->startRs(settingdlg->description(), tm);
        rs->startRs("COM3,9600,N,8,1", tm);
    }
    else
    {
        emit(exit());
        if (rs != nullptr)
            delete rs;
        rs = nullptr;
    }
}

void Dialog::rsStarted()
{
    ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "Старт потока" + "\n");
}

void Dialog::rsFinished()
{
    ui->textEdit->setPlainText(ui->textEdit->toPlainText() + "Завершение потока" + "\n");
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


void Dialog::on_Dialog_finished(int result)
{
    qDebug() << "on_Dialog_finished";
    emit(exit());
    if (rs != nullptr)
        delete rs;
    rs = nullptr;
    qDebug() << "end on_Dialog_finished";
}

QString Dialog::GetHex(QByteArray& array, int maxlength)
{
    QString tmp;
    for (int i=0; i<array.length() && i<maxlength; i++)
    {
        tmp += QString("%1 ").arg((BYTE)array[i],2,16,QChar('0')).toUpper();
    }
    return tmp;
}
