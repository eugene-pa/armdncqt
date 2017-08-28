#include "framemvv.h"
#include "ui_framemvv.h"

FrameMVV::FrameMVV(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameMVV)
{
    ui->setupUi(this);
}

FrameMVV::~FrameMVV()
{
    delete ui;
}
