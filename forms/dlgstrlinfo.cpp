#include <QStandardItemModel>

#include "dlgstrlinfo.h"
#include "ui_dlgstrlinfo.h"
#include "../spr/station.h"

DlgStrlInfo::DlgStrlInfo(Station * st, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStrlInfo)
{
    this->st = st;
    ui->setupUi(this);


    QStandardItemModel * model  = new QStandardItemModel(5, 10, this);

    //QString h("Столбец 1");
    //QStandardItem * item = new QStandardItem ("Столбец 1");
    model->setHorizontalHeaderItem(0, new QStandardItem ("Столбец 1"));

    for (int nTopRow = 0; nTopRow < 5; ++nTopRow)
    {
        QModelIndex index = model->index(nTopRow, 0);
        model->setData(index, "item" + QString::number(nTopRow + 1));

        index = model->index(nTopRow, 1);
        model->setData(index, "subitem" + QString::number(nTopRow + 1));

/*
        model->insertRows(0, 4, index);

        model->insertColumns(0, 3, index);

        for (int nRow = 0; nRow < 4; ++nRow)
        {
            for (int nCol = 0; nCol < 3; ++nCol)
            {
                QString strPos = QString("%1,%2").arg(nRow).arg(nCol);
                model->setData(model->index(nRow, nCol, index), strPos);
            }
        }
*/
    }

    ui->tableStrl->setModel(model);
    ui->tableStrl->show();
    //show();
}

DlgStrlInfo::~DlgStrlInfo()
{
    delete ui;
}
