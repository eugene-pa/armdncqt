#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kpframe.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->gridLayout_kp->addWidget(new kpframe(this),0,0);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,1);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,2);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,3);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,4);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,5);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,6);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,7);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,8);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,9);
    ui->gridLayout_kp->addWidget(new kpframe(this),0,10);
    //ui->gridLayout_kp->addWidget(new kpframe(this),0,11);
    //ui->gridLayout_kp->addWidget(new kpframe(this),0,12);

    ui->gridLayout_kp->addWidget(new kpframe(this),1,0);
    ui->gridLayout_kp->addWidget(new kpframe(this),1,1);
    ui->gridLayout_kp->addWidget(new kpframe(this),1,2);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,3);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,4);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,5);
//    ui->gridLayout_kp->addWidget(new kpframe(this),1,6);

    rasRs = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_triggered()
{
    // 1. создаем класс
    //rasRs = new RasRs(this, 1, 600);
    //connect(rasRs, SIGNAL(dataready(QByteArray)), this, SLOT(dataready(QByteArray)));
    //connect(rasRs, SIGNAL(timeout()), this, SLOT(timeout()));
    //connect(rasRs, SIGNAL(error(int)), this, SLOT(error(int)));
    //connect(rasRs, SIGNAL(started() ), this, SLOT(rsStarted()));
    //connect(rasRs, SIGNAL(finished()), this, SLOT(rsFinished));
    //connect(this, SIGNAL(exit()), rasRs, SLOT(exit()));

    // 2. определяем задержки и стартуем поток
    COMMTIMEOUTS tm = { 10, 1, 3000, 1, 250 };
    //rasRs->startRs("COM3,57600,N,8,1", tm);

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
void MainWindow::error  (int error)
{
//    qDebug() << "Ошибка: " << BlockingRs::errorText(error);
}

// обработка сигнала-уведомления от старте потока RS
void MainWindow::rsStarted()
{
//    qDebug() << "Старт рабочего потока " << rasRs->name();
}


void MainWindow::closeEvent(QCloseEvent *)
{
    if (rasRs)
    {
        emit (exit());
        delete rasRs;
        rasRs = nullptr;
    }
}

void MainWindow::on_pushButtonMainOff_clicked()
{

}

void MainWindow::on_pushButtonRsrvOff_clicked()
{

}

void MainWindow::on_pushButtonToMain_clicked()
{

}

void MainWindow::on_pushButtonToRsrv_clicked()
{

}

void MainWindow::on_pushButtonTest_clicked()
{

}

void MainWindow::on_pushButtonATU_clicked()
{

}

void MainWindow::on_pushButtonReset_clicked()
{

}

void MainWindow::on_pushButtonGetReconnect_clicked()
{

}

void MainWindow::on_pushButtonResetMain_clicked()
{

}

void MainWindow::on_pushButtonResetRsrv_clicked()
{

}

void MainWindow::on_pushButtonWatchdog_clicked()
{

}

void Log (std::wstring s)
{

}
