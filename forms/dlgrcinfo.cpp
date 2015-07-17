#include "dlgrcinfo.h"
#include "ui_dlgrcinfo.h"

DlgRcInfo::DlgRcInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRcInfo)
{
    ui->setupUi(this);

    ui->tableRc->setColumnCount(8);
}

DlgRcInfo::~DlgRcInfo()
{
    delete ui;
}
