#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rasrs.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingdlg = nullptr;
    ui->action_Stop->setEnabled(false);
    rasRs = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Config_triggered()
{
    if (settingdlg==nullptr)
    {
        settingdlg = new SettingsDialog(this);
        connect(settingdlg, SIGNAL(applysettings()),this, SLOT(applysettings()));
    }
    settingdlg->showNormal();
}

void MainWindow::applysettings()
{
     ui->statusBar->showMessage("Параметры порта: " + settingdlg->description(),0);
     ui->action_Start->setText("Старт " + settingdlg->description());
     ui->action_Stop ->setText("Стоп " + settingdlg->description());
}

void MainWindow::on_action_Start_triggered()
{
    ui->action_Start->setEnabled(false);
    ui->action_Stop->setEnabled(true);

    // 1. создаем класс
    rasRs = new RasRS(this, 1, 600);
    connect(rasRs, SIGNAL(dataready(QByteArray)), this, SLOT(dataready(QByteArray)));
    connect(rasRs, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(rasRs, SIGNAL(error(int)), this, SLOT(error(int)));
    connect(rasRs, SIGNAL(started() ), this, SLOT(rsStarted()));
    //connect(rasRs, SIGNAL(finished()), this, SLOT(rsFinished));
    connect(this, SIGNAL(exit()), rasRs, SLOT(exit()));

    // 2. определяем задержки и стартуем поток
    COMMTIMEOUTS tm = { 10, 1, 3000, 1, 250 };
    rasRs->startRs(settingdlg->description(), tm);          // "COM3,57600,N,8,1"

}

void MainWindow::on_action_Stop_triggered()
{
    ui->action_Start->setEnabled(true);
    ui->action_Stop->setEnabled(false);

    emit (exit());
    delete rasRs;

    ui->label->setText("Closed");

}

// обработка сигнала-уведомления о готовности данных
void MainWindow::dataready(QByteArray data)
{
    //ui->statusBar->showMessage(QString("Приняты данные: %1").arg(Logger::GetHex(data, 32)));
    ui->label->setText(QString("Приняты данные: %1").arg(GetHex(data, 32)));
}

// обработка сигнала-уведомления об отсутствии данных в канала данных
void MainWindow::timeout()
{
    ui->statusBar->showMessage("Нет данных");
}

// обработка сигнала-уведомления об ошибке
void MainWindow::error  (int err)
{
    ui->statusBar->showMessage("Ошибка порта: " + BlockingRs::errorText ((BlockingRs::DataError)err));
}

// обработка сигнала-уведомления от старте потока RS
void MainWindow::rsStarted()
{
    ui->statusBar->showMessage("Старт потока");
}

QString MainWindow::GetHex(QByteArray& array, int maxlength)
{
    QString tmp;
    for (int i=0; i<array.length() && i<maxlength; i++)
    {
        tmp += QString("%1 ").arg((BYTE)array[i],2,16,QChar('0')).toUpper();
    }
    return tmp;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    on_action_Stop_triggered();
}
