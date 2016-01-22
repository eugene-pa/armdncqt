#include "bminfoframe.h"
#include "ui_bminfoframe.h"


BmInfoFrame::BmInfoFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BmInfoFrame)
{
    ui->setupUi(this);
    mt.append(ui->label_mt1);
    mt.append(ui->label_mt2);
    mt.append(ui->label_mt3);
    mt.append(ui->label_mt4);
    mt.append(ui->label_mt5);
    mt.append(ui->label_mt6);
    mt.append(ui->label_mt7);
    mt.append(ui->label_mt8);
    mt.append(ui->label_mt9);
    mt.append(ui->label_mt10);
    mt.append(ui->label_mt11);
    mt.append(ui->label_mt12);
    mt.append(ui->label_mt13);
    mt.append(ui->label_mt14);
    mt.append(ui->label_mt15);
    mt.append(ui->label_mt16);
    mt.append(ui->label_mt17);
    mt.append(ui->label_mt18);
    mt.append(ui->label_mt19);
    mt.append(ui->label_mt20);
    mt.append(ui->label_mt21);
    mt.append(ui->label_mt22);
    mt.append(ui->label_mt23);
    mt.append(ui->label_mt24);
    mt.append(ui->label_mt25);
    mt.append(ui->label_mt26);
    mt.append(ui->label_mt27);
    mt.append(ui->label_mt28);
    mt.append(ui->label_mt29);
    mt.append(ui->label_mt30);
    mt.append(ui->label_mt31);
    mt.append(ui->label_mt32);
    mt.append(ui->label_mt33);
    mt.append(ui->label_mt34);
    mt.append(ui->label_mt35);
    mt.append(ui->label_mt36);
    mt.append(ui->label_mt37);
    mt.append(ui->label_mt38);
    mt.append(ui->label_mt39);
    mt.append(ui->label_mt40);
    mt.append(ui->label_mt41);
    mt.append(ui->label_mt42);
    mt.append(ui->label_mt43);
    mt.append(ui->label_mt44);
    mt.append(ui->label_mt45);
    mt.append(ui->label_mt46);
    mt.append(ui->label_mt47);
    mt.append(ui->label_mt48);
}

BmInfoFrame::~BmInfoFrame()
{
    mt.clear();
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
    ui->label_PNP->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsRetrans(rsrv) ? * g_green : * g_gray);

    // Отладочная консоль
    ui->label_consol->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsConsol(rsrv) ? * g_green : * g_gray);

    // БРОК/ОМУЛ
    // - не УПОК+БРОК - серый
    // - не готовность - желтый
    // - готовность ДЦ - зеленый круг
    // - готовность в отладочном режиме(консоль на станции) - зеленый квадрат
    // - выполнение ОТУ - белый круг(ДЦ) или квадрат(ОТЛАДКА)
    ui->label_OTU->setPixmap(!st->IsUpokotu()  ? * g_gray :
                             !st->IsOtuLineOk(rsrv)? * g_yellow :
                              st->IsDebugOtuMode(rsrv) ? st->IsOtuPending(rsrv) ? *g_white_box : *g_green_box :
                                                         st->IsOtuPending(rsrv) ? *g_white     : *g_green);

    // режим АРМ ДСП
    ui->label_ArmDsp->setPixmap(!st->IsSupportKpExt(rsrv) ? * g_gray : st->IsArmDspModeOn(rsrv) ? * g_green : * g_gray);

    // время опроса
    ui->label_last->setText(QString("%1").arg(sysinfo->LatTime().toString("hh:mm:ss")));

    // версия ПО
    ui->label_version->setText(st->Kp2000() ? "КП-2000" : !st->IsSupportKpExt(rsrv) ? "1.0.7.*" : QString("1.0.7.%1").arg(sysinfo->LoVersionNo()));

    // число ошибок связи
    ui->label_errors->setText(QString(" Ошб:%1").arg(sysinfo->LinkErrors()));

    // скорость и реконнекты COM3/COM4
    ui->label_COM3_about->setText(QString("v=%1  #%2").arg(sysinfo->SpeedCom3()).arg(sysinfo->BreaksCom3()));
    ui->label_COM4_about->setText(QString("v=%1  #%2").arg(sysinfo->SpeedCom4()).arg(sysinfo->BreaksCom4()));

    // отображение модулей ДОРАБОТАТЬ
    for (int i=0; i<MaxModule; i++)
    {
        mt[i]->setPixmap(st->IsTsPresent(i) ? *g_green_box : st->IsTuPresent(i) ? *g_yellow_box : *g_gray_box);
    }
}
