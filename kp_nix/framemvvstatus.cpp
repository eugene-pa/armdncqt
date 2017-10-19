#include "framemvvstatus.h"
#include "ui_framemvvstatus.h"

// Панель состояние МВВ 1/2

FrameMvvStatus::FrameMvvStatus(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::FrameMvvStatus)
{
    ui->setupUi(this);

    // инициализация индикаторов
    ui->label_TU1 ->set (QLed::ledShape::box  , QLed::ledStatus::off  , Qt::red);
    ui->label_TU2 ->set (QLed::ledShape::box  , QLed::ledStatus::off  , Qt::red);
    ui->label_OUT1->set (QLed::ledShape::box  , QLed::ledStatus::off  , Qt::red);
    ui->label_OUT2->set (QLed::ledShape::box  , QLed::ledStatus::off  , Qt::red);
    ui->label_ATU1->set (QLed::ledShape::box  , QLed::ledStatus::blink, Qt::red);
    ui->label_ATU2->set (QLed::ledShape::box  , QLed::ledStatus::off  , Qt::red);
}

FrameMvvStatus::~FrameMvvStatus()
{
    delete ui;
}
