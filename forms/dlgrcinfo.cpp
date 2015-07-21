#include "dlgrcinfo.h"
#include "ui_dlgrcinfo.h"
#include "../spr/station.h"

DlgRcInfo::DlgRcInfo(Station * st, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::DlgRcInfo)
{
    this->st = st;
    ui->setupUi(this);

    ui->tableRc->setColumnCount(8);
    ui->tableRc->setRowCount(5);
    ui->tableRc->verticalHeader()->setDefaultSectionSize(20);
    ui->tableRc->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //ui->tableRc->resizeColumnsToContents();

    ui->tableRc->setHorizontalHeaderLabels(QStringList() << "Имя РЦ" << "#" << "Маршрут" << "Поезд" << "ТС" << "ТУ" << "Слева" << "Справа" );
    QPixmap * green   = new QPixmap("../images/icon_grn.ico");
    ui->tableRc->setItem(0,0, new QTableWidgetItem (*green, "OK"));
    ui->tableRc->setItem(4,0, new QTableWidgetItem (*green, "OK"));

    QTableWidgetItem * item = new QTableWidgetItem ("ERROR");
    item->setBackground(Qt::red);
    ui->tableRc->setItem(3,0, item);

}

DlgRcInfo::~DlgRcInfo()
{
    ui->tableRc->clear();
    delete ui;
}

void DlgRcInfo::FillData()
{
    //foreach (Rc * rc, )
}
