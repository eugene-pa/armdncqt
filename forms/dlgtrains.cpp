#include "dlgtrains.h"
#include "../spr/train.h"
#include "../spr/rc.h"
#include "ui_dlgtrains.h"

DlgTrains::DlgTrains(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTrains)
{
    ui->setupUi(this);
    QTableWidget * t = ui->tableTrains;
    t->setColumnCount(3);
    t->setHorizontalHeaderLabels(QStringList() << "sno" << "No" << "РЦ" );
    fill();
}

DlgTrains::~DlgTrains()
{
    delete ui;
}

void DlgTrains::fill(bool all)
{
    QTableWidget * t = ui->tableTrains;
    t->clearContents();
    t->setRowCount(Train::Trains.count());
    t->setSortingEnabled(false);

    int row = 0;
    foreach (Train * train, Train::Trains)
    {
        if (!all && train->no==0)
            continue;
        t->setItem(row,0, new QTableWidgetItem (QString::number(train->sno)));
        t->setItem(row,1, new QTableWidgetItem (QString::number(train->no)));
        QString rc;
        for (int i=0; i<train->nrc; i++)
        {
            rc += QString("%1  ").arg(train->Rc[i]->NameEx());
        }
        t->setItem(row,2, new QTableWidgetItem (rc));

        row++;
    }
    t->setRowCount(row);

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(1, Qt::AscendingOrder);             // сортировка по умолчанию по №

}

void DlgTrains::on_checkBox_toggled(bool checked)
{
    fill(checked);
}
