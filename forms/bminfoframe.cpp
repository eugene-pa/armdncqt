#include "bminfoframe.h"
#include "ui_bminfoframe.h"


BmInfoFrame::BmInfoFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::BmInfoFrame)
{
    ui->setupUi(this);

    okColor = QColor(0,220,0);
    erColor = QColor(255,0,0);
    okBrush = QBrush(okColor);
    erBrush = QBrush(erBrush);

    // динамически создаем матрицу МТ с разметкой
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            bool labels = i==3 || i==7;
            QLabel * label = labels ? new QLabel(QString::number(j+1),this) : nullptr;
            QLed   * led   =!labels ? new QLed(this, QLed::box, QLed::on, QColor(0,196,0,255), Qt::gray) : nullptr;

            QWidget * w = label == nullptr ? led : label;
            w->setGeometry(QRect(0, 0, 8, 8));
            ui->gridLayout->addWidget(w,i,j);

            if (led != nullptr)
                mt.append(led);
            else
                label->setAlignment(Qt::AlignCenter);
        }
    }



}

BmInfoFrame::~BmInfoFrame()
{
//    for (int i=0; i<mt.length(); i++)
//        delete mt[i];
//    mt.clear();
    delete ui;
}


// назначить объект
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


// обработка тика таймера
void BmInfoFrame::timerEvent(QTimerEvent *event)
{
    redraw();
}


// TODO: показать существующие модули ТУ/ТС
// отрисовка неопределенного состояния модуля (нет данных)
void BmInfoFrame::RedrawUndefined()
{
    ui->label_ready ->set(QLed::round, QLed::off);
    ui->label_actual->set(QLed::round, QLed::off);
    ui->label_COM3  ->set(QLed::round, QLed::off);
    ui->label_COM4  ->set(QLed::round, QLed::off);
    ui->label_ATU   ->set(QLed::round, QLed::off);
    ui->label_Key   ->set(QLed::round, QLed::off);
    ui->label_Out   ->set(QLed::round, QLed::off);
    ui->label_Test  ->set(QLed::round, QLed::off);
    ui->label_Timer ->set(QLed::round, QLed::off);
    ui->label_Mem   ->set(QLed::round, QLed::off);
    ui->label_PNP   ->set(QLed::round, QLed::off);
    ui->label_OTU   ->set(QLed::round, QLed::off);
    ui->label_ArmDsp->set(QLed::round, QLed::off);
    ui->label_consol->set(QLed::round, QLed::off);

    for (int i=0; i<MaxModule; i++)
    {
        mt[i]->set(QLed::box, st->IsTsPresent(i) || st->IsTuPresent(i) ? QLed::on : QLed::off, QBrush(Qt::gray), Qt::NoBrush);
        if (st->IsTuPresent(i))
            mt[i]->setText("У");
    }


}


void BmInfoFrame::redraw()
{
    if (st->IsTsExpire())
    {
        RedrawUndefined();
        return;
    }
    // Основной/резервный
    ui->label_actual->set(QLed::round, QLed::on, rsrv ?  st->IsRsrv() ? okBrush : Qt::gray
                                                      : !st->IsRsrv() ? okBrush : Qt::gray );
    // готовность БМ    TODO!!!
    ui->label_ready->set(QLed::round, QLed::on, okBrush);

    // COM3
    ui->label_COM3->set(QLed::round, QLed::on, sysinfo->Com3Connected() ? okBrush : sysinfo->Com3Error() ? erBrush : Qt::yellow);

    // COM4
    ui->label_COM4->set(QLed::round, QLed::on, sysinfo->Com4Connected() ? okBrush : sysinfo->Com3Error() ? erBrush : Qt::yellow);


    // АТУ
    ui->label_ATU->set(QLed::round, QLed::on, st->IsAtuError (rsrv) ? erBrush : okBrush);

    // Ключ
    ui->label_Key->set(QLed::round, QLed::on, st->Kp2000() ? Qt::NoBrush : st->IsKeyError (rsrv) ? erBrush : okBrush);

    // Выход
    ui->label_Out->set(QLed::round, QLed::on, st->Kp2000() ? Qt::NoBrush : st->IsOutError (rsrv) ? erBrush : okBrush);

    // Тест
    ui->label_Test->set(QLed::round, QLed::on, st->Kp2000() ? Qt::NoBrush : st->IsTestMode(rsrv) ? QBrush(Qt::white) : Qt::NoBrush);

    // Таймер
    ui->label_Timer->set(QLed::round, QLed::on, !st->IsSupportKpExt(rsrv) ? Qt::NoBrush : st->IsWatchdogOn(rsrv) ? okBrush : * g_gray);

    // Память
    ui->label_Mem->set(QLed::round, QLed::on, !st->IsSupportKpExt(rsrv) ? Qt::NoBrush : st->IsMemError(rsrv) ? * g_red : okBrush);

    // Ретрансляция
    ui->label_PNP->set(QLed::round, QLed::on, !st->IsSupportKpExt(rsrv) ? Qt::NoBrush : st->IsRetrans(rsrv) ? okBrush : * g_gray);

    // Отладочная консоль
    ui->label_consol->set(QLed::round, QLed::on, !st->IsSupportKpExt(rsrv) ? Qt::NoBrush : st->IsConsol(rsrv) ? okBrush : * g_gray);

    // БРОК/ОМУЛ
    // - не УПОК+БРОК - серый
    // - не готовность - желтый
    // - готовность ДЦ - зеленый
    // - готовность в отладочном режиме - белый квадрат
    // - выполнение ОТУ - мигание зеленого или белого
    ui->label_OTU->set(QLed::round, st->IsOtuPending(rsrv) ? QLed::blink : QLed::on, !st->IsUpokotu()      ? Qt::NoBrush            :
                                                                                     !st->IsOtuLineOk(rsrv)? QBrush(Qt::yellow)     :
                                                                                      st->IsDebugOtuMode(rsrv) ? QBrush (Qt::white) : okBrush);

    // режим АРМ ДСП
    ui->label_ArmDsp->set(QLed::round, QLed::on, !st->IsSupportKpExt(rsrv) ? Qt::NoBrush : st->IsArmDspModeOn(rsrv) ? okBrush : * g_gray);

    // время опроса
    ui->label_last->setText(QString("%1").arg(sysinfo->LatTime().toString("hh:mm:ss")));

    // версия ПО
    ui->label_version->setText(st->Kp2000() ? "КП-2000" : !st->IsSupportKpExt(rsrv) ? "1.0.7.*" : QString("1.0.7.%1").arg(sysinfo->LoVersionNo()));

    // число ошибок связи
    ui->label_errors->setText(QString(" Ошб:%1").arg(sysinfo->LinkErrors()));

    // скорость и реконнекты COM3/COM4
    ui->label_COM3_about->setText(QString("v=%1  #%2").arg(sysinfo->SpeedCom3()).arg(sysinfo->BreaksCom3()));
    ui->label_COM4_about->setText(QString("v=%1  #%2").arg(sysinfo->SpeedCom4()).arg(sysinfo->BreaksCom4()));

    // отображение модулей
    for (int i=0; i<mt.length(); i++)
    {
        bool er = sysinfo->GetMtuMtsLineStatus(i);
        mt[i]->set(QLed::box, st->IsTsPresent(i) || st->IsTuPresent(i) ? QLed::on : QLed::off, er ? erBrush : okBrush, Qt::NoBrush);
        if (st->IsTuPresent(i))
            mt[i]->setText("У");
    }

    update();                                               // без этого не отображает изменения...
}
