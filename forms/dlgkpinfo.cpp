#include "dlgkpinfo.h"
#include "ui_dlgkpinfo.h"
#include "qled.h"

DlgKPinfo::DlgKPinfo(class Station * p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgKPinfo)
{
    ui->setupUi(this);

    okColor = QColor(0,220,0);
    redraw(p);

    startTimer (1000);
}

DlgKPinfo::~DlgKPinfo()
{
    delete ui;
}

// смена станции
void DlgKPinfo::changeStation(class Station * p)
{
    if (p!=st)
    {
        redraw (p);
    }
}

void DlgKPinfo::redraw(Station * p)
{
    if (p==nullptr)
        return;
    if (p!=st)
    {
        st = p;
        setWindowTitle(QString ("Состояние КП ст.%1 (#%2  адр=%3[%4], id=%5:%6)").arg(st->Name()).arg(st->No()).arg(st->Addr()).arg(st->Ras()).arg(st->GidUralId()).arg(st->GidRemoteId()));
        ui->frame_Main->setObj(st, false);
        ui->frame_Rsrv->setObj(st, true );
        ui->label_st->setText (st->Name());
    }

    bool rsrv = st->IsRsrv();
    SysInfo * sysinfo = st->GetSysInfo(rsrv);
    ui->label_COM3      ->set(QLed::round, QLed::on, st->IsBackChannel() ? Qt::NoBrush : QBrush(okColor));
    ui->label_COM4      ->set(QLed::round, QLed::on,!st->IsBackChannel() ? Qt::NoBrush : QBrush(okColor));

    // В существующей версии оценка работоспособности сети оценивается путем сравнения
    QDateTime to = st->GetLastTime (false),
              tr = st->GetLastTime (true);
    bool netSts = qAbs(to.secsTo(tr)) < 10;
    ui->label_NET       ->set(QLed::round, QLed::on,!st->IsSupportKpExt(rsrv) ? Qt::NoBrush : netSts ? QBrush(okColor) : QBrush(Qt::yellow));

    // сравнение ТС = ОК, если сеть ОК и флаг сравнения ОК
    ui->label_CompareTS ->set(QLed::round, QLed::on,!st->IsSupportKpExt(rsrv) ? Qt::NoBrush : netSts && sysinfo->TsCompared() ? QBrush(okColor) : QBrush(Qt::yellow));

    ui->label_OTU        ->set(QLed::round, QLed::on, QBrush(okColor));
    ui->label_MPC        ->set(QLed::round, QLed::on,!st->IsMpcEbilock() ? Qt::NoBrush : sysinfo->IsMpcOk () ? QBrush(okColor) : QBrush(Qt::red));
    ui->label_RPC        ->set(QLed::round, QLed::on,!st->IsRpcDialog()  ? Qt::NoBrush : sysinfo->IsRpcOk () ? QBrush(okColor) : QBrush(Qt::red));
    ui->label_ADKSCB     ->set(QLed::round, QLed::on,!st->IsAdkscb()     ? Qt::NoBrush : sysinfo->OkAdkScb() ? QBrush(okColor) : QBrush(Qt::red));
    ui->label_DK         ->set(QLed::round, QLed::on,!(st->IsApkdk() || st->IsAbtcm()) ? Qt::NoBrush : sysinfo->OkApkDk() ? QBrush(okColor) : QBrush(Qt::red));
}

void DlgKPinfo::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        update();
}

void DlgKPinfo::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    redraw();
}
