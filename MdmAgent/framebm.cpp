#include "framebm.h"
#include "ui_framebm.h"
#include "../spr/station.h"

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


// показать актуальную информацию по станции
void FrameBM::Show()
{

}
