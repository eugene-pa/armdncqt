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

    ui->treeStrl->setColumnCount(3);
    ui->treeStrl->setHeaderLabel("Состояние стрелок по станции " + st->Name());
    ui->treeStrl->setHeaderLabels(QStringList() << "Имя стрелки" << "Состояние");

    for (int i = 0; i < 5; ++i)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << "Стрелка " + QString::number(i) << "Столбец");
        ui->treeStrl->addTopLevelItem(item);
        QTreeWidgetItem * itemplus  = new QTreeWidgetItem(QStringList() << "ТС");
        QTreeWidgetItem * itemminus = new QTreeWidgetItem(QStringList() << "ТУ");
        item->addChild(itemplus);
        item->addChild(itemminus);
        itemplus ->addChild(new QTreeWidgetItem(QStringList() << "+" << "плюс"));
        itemplus ->addChild(new QTreeWidgetItem(QStringList() << "-" << "минус"));
        itemminus->addChild(new QTreeWidgetItem(QStringList() << "+" << "плюс"));
        itemminus->addChild(new QTreeWidgetItem(QStringList() << "-" << "минус"));
        itemplus->setBackground(1,Qt::green);
        itemminus->setBackground(1,Qt::yellow);
    }
}

DlgStrlInfo::~DlgStrlInfo()
{
    delete ui;
}
