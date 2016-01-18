#include "dlgkpinfo.h"
#include "ui_dlgkpinfo.h"
#include "../forms/bminfowidget.h"

DlgKPinfo::DlgKPinfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgKPinfo)
{
    ui->setupUi(this);
}

DlgKPinfo::~DlgKPinfo()
{
    delete ui;
}
