#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mutex"
#include "../common/logger.h"
#include "../common/sqlblackbox.h"

std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения


QString mainstr = "DRIVER=QPSQL;Host=192.168.0.107;PORT=5432;DATABASE=blackbox;USER=postgres;PWD=358956";
QString rsrvstr = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    exit_lock.lock();                                       // блокируем мьютекс завершения
    ui->setupUi(this);

    blackbox = new SqlBlackBox (mainstr, rsrvstr, logger = new Logger("LOG/trstpsql.log", true, true));
    blackbox->putMsg(1, "Тест записи 1", APP_MONITOR , LOG_TU);
    blackbox->putMsg(3, "Тест записи 3", APP_MDMAGENT, LOG_NOTIFY);

}

MainWindow::~MainWindow()
{ 
    exit_lock.unlock();                                     // разблокируем мьютекс завершения
    delete blackbox;
    delete ui;
}
