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
    connect(&workerThread1, SIGNAL(finished()), waiter1, SLOT(deleteLater()));      // стандартный сигнал/слот
    connect(this   , SIGNAL(wait(int))   , waiter1, SLOT(waiting(int)));
    connect(waiter1, SIGNAL(timeout())   , this   , SLOT(slotTimeout1()));
    connect(waiter1, SIGNAL(evented(int)), this   , SLOT(slotEvent1(int)));

    // поток 2
    Water * waiter2 = new Water(condition);
    waiter2->moveToThread(&workerThread2);
    connect(&workerThread2, SIGNAL(finished()), waiter2, SLOT(deleteLater()));      // стандартный сигнал/слот
    connect(this   , SIGNAL(wait(int))   , waiter2, SLOT(waiting(int)));
    connect(waiter2, SIGNAL(timeout())   , this   , SLOT(slotTimeout2()));
    connect(waiter2, SIGNAL(evented(int)), this   , SLOT(slotEvent2(int)));

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

