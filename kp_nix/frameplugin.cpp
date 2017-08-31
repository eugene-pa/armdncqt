#include "frameplugin.h"
#include "ui_frameplugin.h"

FramePlugin::FramePlugin(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::FramePlugin)
{
    ui->setupUi(this);
}

FramePlugin::~FramePlugin()
{
    delete ui;
}
