#include <QMdiArea>
#include <QMdiSubWindow>
#include <QGroupBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMdiArea * mdiArea = new QMdiArea;                                    // создаем виджет MDI
    //mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);       // скролбары
    //mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //QGroupBox * mdiArea = new QGroupBox;
    //mdiArea->setTitle("1111111");
    setCentralWidget(mdiArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}
