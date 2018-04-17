#include "mainwindow.h"
#include "kpframe.h"
#include "ui_kpframe.h"

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
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_Main->setStyleSheet("color: rgb(0, 64, 0)");
    ui->label_Rsrv->setStyleSheet("color: rgb(128, 128, 128)");
    ui->pushButton->setText(st->Name().left(8));

}

kpframe::~kpframe()
{
    delete ui;
}

void kpframe::SetActual(bool s, bool rsrv)
{
    QLed * l = st->IsBackChannel() ? ui->label_mainCOM4 : ui->label_mainCOM3;
    l->set (QLed::ledShape::box, s ? QLed::ledStatus::on : QLed::ledStatus::off, Qt::white);
}

// отрисовка актуального динамического состояния КП
void kpframe::Show()
{
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::green);
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::green);
    ui->label_rsrvCOM3->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::darkGreen);
    ui->label_rsrvCOM4->set (QLed::ledShape::box, QLed::ledStatus::on, Qt::darkGreen);

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
