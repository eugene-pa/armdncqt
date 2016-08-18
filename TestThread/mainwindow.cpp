#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "water.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    condition = new QWaitCondition();

    // поток 1
    Water * waiter1 = new Water(condition);
    waiter1->moveToThread(&workerThread1);
    connect(&workerThread1, &QThread::finished, waiter1, &QObject::deleteLater);
    connect(this, &MainWindow::wait, waiter1, &Water::waiting);
    connect(waiter1, &Water::timeout, this, &MainWindow::slotTimeout1);
    connect(waiter1, &Water::evented, this, &MainWindow::slotEvent1);

    // поток 2
    Water * waiter2 = new Water(condition);
    waiter2->moveToThread(&workerThread2);

    connect(&workerThread2, &QThread::finished, waiter2, &QObject::deleteLater);
    connect(this, &MainWindow::wait, waiter2, &Water::waiting);
    connect(waiter2, &Water::timeout, this, &MainWindow::slotTimeout2);
    connect(waiter2, &Water::evented, this, &MainWindow::slotEvent2);

    workerThread1.start();
    workerThread2.start();

    emit wait(5000);                        // стартуем функции потоков

}

MainWindow::~MainWindow()
{
    Water::rqfinish = true;                 // флаг завершения
    condition->wakeAll();                   // прерываем ожидание

    // завершение потоков из примера
    workerThread1.quit();
    workerThread2.quit();
    workerThread1.wait();
    workerThread2.wait();

    delete condition;

    delete ui;
}

// генерация события
void MainWindow::on_pushButton_clicked()
{
    condition->wakeAll();
}

// обработка уведомления о тайм-ауте
void MainWindow::slotTimeout1()
{
    ui->lineEdit->setText(QDateTime::currentDateTime().toString("hh:mm:ss. ") + "Истек таймаут ожидания события в потоке");
}

// обработка уведомления о событии
void MainWindow::slotEvent1(int n)
{
    ui->lineEdit->setText(QDateTime::currentDateTime().toString("hh:mm:ss. ") + "Поток получил событие "  + QString::number(n));
}

// обработка уведомления о тайм-ауте
void MainWindow::slotTimeout2()
{
    ui->lineEdit_2->setText(QDateTime::currentDateTime().toString("hh:mm:ss. ") + "Истек таймаут ожидания события в потоке");
}

// обработка уведомления о событии
void MainWindow::slotEvent2(int n)
{
    ui->lineEdit_2->setText(QDateTime::currentDateTime().toString("hh:mm:ss. ") + "Поток получил событие " + QString::number(n));
}

