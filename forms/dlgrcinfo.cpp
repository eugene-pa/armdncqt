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
    //QTableWidgetItem * h = ui->tableRc->horizontalHeaderItem(0);
    //h ->setText("Имя РЦ");
    int a = 99;
    ui->tableRc->setHorizontalHeaderLabels(QStringList() << "Имя РЦ" << "#" << "Маршрут" << "Поезд" << "ТС" << "ТУ" << "Слева" << "Справа" );
    QTableWidgetItem * item = new QTableWidgetItem ("0,0");
    ui->tableRc->setItem(1,1, item);

}

DlgRcInfo::~DlgRcInfo()
{
    delete ui;
}

void DlgRcInfo::FillData()
{
    //foreach (Rc * rc, )
}
