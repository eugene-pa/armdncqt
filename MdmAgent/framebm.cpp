#include "framebm.h"
#include "ui_framebm.h"
#include "../spr/station.h"

FrameBM::FrameBM(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameBM)
{
    ui->setupUi(this);
}

FrameBM::~FrameBM()
{
    delete ui;
}


// показать актуальную информацию по станции
void FrameBM::Show(class Station * pst)
{
    st = pst;
    SysInfo * info = st->GetSysInfo(st->IsRsrv());
    ui->label_version->setText(QString("ver.1.0.7.%1").arg(info->LoVersionNo()));
    ui->label_Speed3->setText(QString::number(info->SpeedCom3()));
    ui->label_Speed4->setText(QString::number(info->SpeedCom4()));
    ui->label_break3->setText(QString("#%1").arg(info->BreaksCom3()));
    ui->label_break4->setText(QString("#%1").arg(info->BreaksCom4()));
    ui->label_errors->setText(QString::number(info->LinkErrors()));
}
