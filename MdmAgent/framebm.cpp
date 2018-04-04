#include "framebm.h"
#include "ui_framebm.h"

FrameBM::FrameBM(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameBM)
{
    ui->setupUi(this);
}

FrameBM::~FrameBM()
{
    delete ui;
}
