#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkDatagram>
#include <QUdpSocket>

QUdpSocket * sockRcv;
QUdpSocket * sockSnd;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString a = "127.0.0.1";
    QHostAddress addr(a);

    sockRcv = new QUdpSocket(this);
    sockRcv->bind(addr, 64466);

    connect(sockRcv, SIGNAL(readyRead()), this, SLOT(on_readyRead()));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray bt("123456789");
    QString a = "127.0.0.1";
    QHostAddress addr(a);

    QNetworkDatagram dtgr (bt, addr, 64465);
    sockSnd = new QUdpSocket(this);
    sockSnd->writeDatagram(dtgr);

}


void MainWindow::on_readyRead()
{
    QNetworkDatagram dtgr = sockRcv->receiveDatagram();
    ui->label_rcv->setText(QString(dtgr.data()));
}
