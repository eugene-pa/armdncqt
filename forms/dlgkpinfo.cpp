#include "dlgkpinfo.h"
#include "ui_dlgkpinfo.h"
#include "../forms/bminfowidget.h"

DlgKPinfo::DlgKPinfo(class Station * p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgKPinfo)
{
    st = p;
    ui->setupUi(this);

    ui->frame_Main->setObj(st, false);
    ui->frame_Rsrv->setObj(st, true );
    ui->label_st->setText (st->Name());
}

DlgKPinfo::~DlgKPinfo()
{
    delete ui->frame_Main;                                  // почему-то здесь нужно удалять детей
    delete ui->frame_Rsrv;
    delete ui;
}
