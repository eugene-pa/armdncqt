#include "dlgtrains.h"
#include "ui_dlgtrains.h"

DlgTrains::DlgTrains(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTrains)
{
    ui->setupUi(this);
}

DlgTrains::~DlgTrains()
{
    delete ui;
}

void DlgTrains::fill()
{
    ui->tableTrains->setColumnCount(4);
    ui->tableTrains->setHorizontalHeaderLabels(QStringList() << "sno" << "No" << "РЦ" );
    ui->tableTrains->setSortingEnabled(false);

}
