#include "framebase.h"
#include "ui_framebase.h"

FrameBase::FrameBase(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameBase)
{
    ui->setupUi(this);

    // инициализация индикаторов (изменение делается в коде аналогично)
    ui->labelReady    ->set (QLed::ledShape::box  , QLed::ledStatus::blink, Qt::yellow);
    ui->labelBypass   ->set (QLed::ledShape::box  , QLed::ledStatus::on);
    ui->labelReady2   ->set (QLed::ledShape::box  , QLed::ledStatus::on);
    ui->label_testMTU ->set (QLed::ledShape::box  , QLed::ledStatus::off, Qt::white);
    ui->label_testMTS ->set (QLed::ledShape::box  , QLed::ledStatus::off, Qt::white);
    ui->label_Watchdog->set (QLed::ledShape::round, QLed::ledStatus::on, Qt::white);
}

FrameBase::~FrameBase()
{
    delete ui;
}
