#include "mainwindow.h"
#include "kpframe.h"
#include "ui_kpframe.h"
#include "../spr/sysinfo.h"

kpframe::kpframe(QWidget *parent, Station* pst) :
    QFrame(parent),
    ui(new Ui::kpframe)
{
    ui->setupUi(this);
    st = pst;                                               // сссылка на станцию в виджете
    st->userData = this;                                    // перекрестная ссылка на виджет в станции
    mainWindow = (MainWindow *) parent;

    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_rsrvCOM3->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_rsrvCOM4->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::off, Qt::gray, Qt::darkGray);
    ui->label_Main->setStyleSheet("color: rgb(0, 64, 0)");
    ui->label_Rsrv->setStyleSheet("color: rgb(128, 128, 128)");
    ui->pushButton->setText(st->Name().left(8));
    ui->checkBox->setChecked(st->Enable());

    QPalette o(ui->frame_4->palette());
    o.setColor(QPalette::Window, QColor(128,196,128,64));
    o.setColor(QPalette::Foreground, Qt::darkGray);
    ui->frame_4->setAutoFillBackground(true);
    ui->frame_4->setPalette(o);

    QPalette r(ui->frame_3->palette());
    r.setColor(QPalette::Window, QColor(128,196,128,64));
    r.setColor(QPalette::Foreground, Qt::darkGray);
    ui->frame_3->setAutoFillBackground(true);
    ui->frame_3->setPalette(r);
}

kpframe::~kpframe()
{
    delete ui;
}

void kpframe::SetActual(bool s, bool rsrv, bool NotEmpty)
{
    QLed * l = st->IsBackChannel() ? (rsrv ? ui->label_rsrvCOM4 : ui->label_mainCOM4) : (rsrv ? ui->label_rsrvCOM3 : ui->label_mainCOM3);
    l->set (QLed::ledShape::box, s ? QLed::ledStatus::on : QLed::ledStatus::off, NotEmpty ? Qt::black : Qt::white);
    //l->setText("+");
}

// отрисовка актуального динамического состояния КП
void kpframe::Show()
{
    // пока не учитывается состояние коннекта модема; при необхоимости дать цвет; можно выделить функцию получения цвета заданного модема
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, getColor(false, false));
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, getColor(false, true ));
    ui->label_rsrvCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, getColor(true , false));
    ui->label_rsrvCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, getColor(true , true ));

    if (!st->IsOtuLineOk())
        SetOtuLed(false, false);


/*
    QPalette pal = palette();
    //QBrush brush(Qt::darkGreen);
    pal.setBrush(QPalette::Background, QBrush(Qt::darkGreen));
    pal.setBrush(QPalette::Foreground, QBrush(Qt::darkGreen));
    ui->frame_4->setPalette(pal);
*/
}

// получить цвет индикатора заданного модема
// отсутствие отклика КП - все модемы красные
// при наличии отулика состояние коннекта и ошибки модема берется из SysInfo
QColor kpframe::getColor(bool rsrv, bool com4)
{
    bool active = !(rsrv ^ st->IsRsrv());
    SysInfo * sys = st->GetSysInfo(rsrv);
    bool connect = com4 ? sys->Com4Connected() : sys->Com3Connected();
    bool error   = com4 ? sys->Com4Error    () : sys->Com3Error    ();
    return !st->IsKpResponce()  ? Qt::red   :
           connect              ? (active ? Qt::green : Qt::white) :
           error                ? Qt::red   : Qt::yellow;
}

// обработка щелчка по станции (смена станции)
void kpframe::on_pushButton_clicked()
{
    emit mainWindow->SelectStation(st);
}

// публичная функция выделения/отмены выделения станции
void kpframe::SelectSt(bool s)
{
    ui->pushButton->setChecked(s);
}

void kpframe::on_checkBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    st->SetEnable(ui->checkBox->isChecked());
}

void kpframe::SetOtuLed(bool ok, bool blink)
{
    ui->label_OTU ->set (QLed::ledShape::box, blink ? QLed::ledStatus::blink_once : QLed::ledStatus::off, (ok ? Qt::green : Qt::red), (ok ? Qt::darkGreen : Qt::darkRed));
}

// мигнуть актуальным цветом
void kpframe::BlinkOtu()
{
    ui->label_OTU ->set (QLed::ledShape::box, QLed::ledStatus::blink_once, ui->label_OTU->Fore(), ui->label_OTU->Back());
}
