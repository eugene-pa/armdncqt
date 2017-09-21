#include "farmeline.h"
#include "ui_farmeline.h"

FarmeLine::FarmeLine(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::FarmeLine)
{
    ui->setupUi(this);

    ui->label_COM->set (QLed::ledShape::box  , QLed::ledStatus::on);
}

FarmeLine::~FarmeLine()
{
    delete ui;
}
