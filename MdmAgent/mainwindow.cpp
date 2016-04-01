#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rasrs.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rasRs = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{
    // 1. создаем класс
    rasRs = new RasRs(this, 1, 600);
    connect(rasRs, SIGNAL(dataready(QByteArray)), this, SLOT(dataready(QByteArray)));
    connect(rasRs, SIGNAL(timeout()), this, SLOT(timeout()));
    connect(rasRs, SIGNAL(error(int)), this, SLOT(error(int)));
    connect(rasRs, SIGNAL(started() ), this, SLOT(rsStarted()));
    //connect(rasRs, SIGNAL(finished()), this, SLOT(rsFinished));
    connect(this, SIGNAL(exit()), rasRs, SLOT(exit()));

    // 2. определяем задержки и стартуем поток
    COMMTIMEOUTS tm = { 10, 1, 3000, 1, 250 };
    rasRs->startRs("COM3,57600,N,8,1", tm);

}

// обработка сигнала-уведомления о готовности данных
void MainWindow::dataready(QByteArray data)
{
    ui->statusBar->showMessage(QString("Приняты данные: %1").arg(Logger::GetHex(data, 32)));
}

// обработка сигнала-уведомления об отсутствии данных в канала данных
void MainWindow::timeout()
{
    ui->statusBar->showMessage("Нет данных");
}

// обработка сигнала-уведомления об ошибке
void MainWindow::error  (int)
{
    int a = 99;
}

// обработка сигнала-уведомления от старте потока RS
void MainWindow::rsStarted()
{
    int a = 99;
}

// завершение потока RS
//void MainWindow::rsFinished()
//{
//    int a = 99;
//}

void MainWindow::on_MainWindow_destroyed()
{
    emit (exit());
    if (rasRs != nullptr)
        delete rasRs;
}
