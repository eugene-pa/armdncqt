#include "bminfowidget.h"
#include "ui_bminfowidget.h"
#include "../common/defines.h"

bmInfoWidget::bmInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bmInfoWidget)
{
    ui->setupUi(this);
    updateKp();
}

bmInfoWidget::~bmInfoWidget()
{
    delete ui;
}

void bmInfoWidget::updateKp()
{
    ui->label_ready->setPixmap(*g_green);
    ui->label_mt1->setPixmap(*g_green_box);
    ui->label_mt2->setPixmap(*g_green_box);
}
