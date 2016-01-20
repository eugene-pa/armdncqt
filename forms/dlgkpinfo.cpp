#include "dlgkpinfo.h"
#include "ui_dlgkpinfo.h"

DlgKPinfo::DlgKPinfo(class Station * p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgKPinfo)
{
    ui->setupUi(this);
    redraw(p);
}

DlgKPinfo::~DlgKPinfo()
{
    delete ui->frame_Main;                                  // почему-то здесь нужно удалять детей
    delete ui->frame_Rsrv;
    delete ui;
}


void DlgKPinfo::redraw(Station * p)
{
    if (p!=nullptr)
    {
        st = p;
        setWindowTitle(QString ("Состояние КП ст.%1 (адр=%2[%3], id=%4:%5)").arg(st->Name()).arg(st->Addr()).arg(st->Ras()).arg(st->GidUralId()).arg(st->GidRemoteId()));
        ui->frame_Main->setObj(st, false);
        ui->frame_Rsrv->setObj(st, true );
        ui->label_st->setText (st->Name());
    }
    //ui->label_ArmDsp->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsArmDspModeOn(rsrv) ? * g_green : * g_gray);
    bool rsrv = st->IsRsrv();
    SysInfo * sysinfo = st->GetSysInfo(rsrv);
    ui->label_COM3      ->setPixmap( st->IsBackChannel() ? * g_gray : * g_green);
    ui->label_COM4      ->setPixmap(!st->IsBackChannel() ? * g_gray : * g_green);

    bool netSts = false;
    ui->label_NET       ->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : netSts ? * g_green : * g_yellow);
    ui->label_CompareTS ->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : sysinfo->TsCompared() ? * g_green : * g_yellow);

    //label_OTU
    ui->label_MPC        ->setPixmap(!st->IsMpcEbilock() ? * g_gray : sysinfo->IsMpcOk () ? * g_green : * g_red);
    ui->label_RPC        ->setPixmap(!st->IsRpcDialog()  ? * g_gray : sysinfo->IsRpcOk () ? * g_green : * g_red);
    ui->label_ADKSCB     ->setPixmap(!st->IsAdkscb()     ? * g_gray : sysinfo->OkAdkScb() ? * g_green : * g_red);
    ui->label_DK         ->setPixmap(!(st->IsApkdk() || st->IsAbtcm()) ? * g_gray : sysinfo->OkApkDk() ? * g_green : * g_red);
}

