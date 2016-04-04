#include "kpframe.h"
#include "ui_kpframe.h"

kpframe::kpframe(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::kpframe)
{
    ui->setupUi(this);
    ui->label_mainCOM3->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_mainCOM4->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_rsrvCOM3->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_rsrvCOM4->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_OTU     ->set (QLed::ledShape::box, QLed::ledStatus::off);
    ui->label_Main->setStyleSheet("color: rgb(0, 64, 0)");
    ui->label_Rsrv->setStyleSheet("color: rgb(128, 128, 128)");

}

kpframe::~kpframe()
{
    delete ui;
}
