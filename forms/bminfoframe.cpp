#include "bminfoframe.h"
#include "ui_bminfoframe.h"

BmInfoFrame::BmInfoFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BmInfoFrame)
{
    ui->setupUi(this);

}

BmInfoFrame::~BmInfoFrame()
{
    delete ui;
}

void setObj(class Station * p, bool s = false);         // назначить объект
void BmInfoFrame::setObj(class Station * p, bool s)
{
    rsrv = s;
    st = p;
    sysinfo = st->GetSysInfo (rsrv);
    if (s)
        ui->label_MainRsrv->setText("Резервный");

    redraw();
    startTimer (1000);
}

void BmInfoFrame::timerEvent(QTimerEvent *event)
{
    redraw();
}

void BmInfoFrame::redraw()
{
    // Основной/резервный
    ui->label_actual->setPixmap(rsrv ?  st->IsRsrv() ? *g_green : *g_gray
                                     : !st->IsRsrv() ? *g_green : *g_gray );
    // готовность БМ
    //ui->label_ready->setPixmap

    // COM3
    ui->label_COM3->setPixmap(sysinfo->Com3Connected() ? *g_green : sysinfo->Com3Error() ? *g_red : * g_yellow);

    // COM4
    ui->label_COM4->setPixmap(sysinfo->Com4Connected() ? *g_green : sysinfo->Com4Error() ? *g_red : * g_yellow);

    // АТУ
    ui->label_ATU->setPixmap(st->IsAtuError (rsrv) ? *g_red : * g_green);

    // Ключ
    ui->label_Key->setPixmap(st->Kp2000() ? * g_gray : st->IsKeyError (rsrv) ? *g_red : * g_green);

    // Выход
    ui->label_Out->setPixmap(st->Kp2000() ? * g_gray : st->IsOutError (rsrv) ? *g_red : * g_green);

    // Тест
    ui->label_Test->setPixmap(st->Kp2000() ? * g_gray : st->IsTestMode(rsrv) ? *g_white : * g_gray);

    // Таймер
    ui->label_Timer->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsWatchdogOn(rsrv) ? * g_green : * g_gray);

    // Память
    ui->label_Mem->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsMemError(rsrv) ? * g_red : * g_green);

    // Ретрансляция
    //ui->label_PNP->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsRetrans(rsrv) ? * g_red : * g_green);

    // Отладочная консоль
    //ui->label_consol->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsConsol(rsrv) ? * g_red : * g_green);

    // БРОК/ОМУЛ
    //ui->label_OTU->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsOtuOk(rsrv) ? * g_red : * g_green);

}
