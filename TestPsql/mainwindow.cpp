#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mutex"

std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    exit_lock.lock();                                       // блокируем мьютекс завершения
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    exit_lock.unlock();                                     // разблокируем мьютекс завершения
    delete ui;
}
