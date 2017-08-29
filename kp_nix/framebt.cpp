#include "framebt.h"
#include "ui_framebt.h"

// Панель "Состояние модулей ТУ/ТС" (включает 2 фрейма для каждого БТ)
// В принципе, можно было бы напрямую прописать два фрейма непосредственно в главном окне

FrameBT::FrameBT(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::FrameBT)
{
    ui->setupUi(this);

    // инициализируем номера МВВ для двух фреймов
    ui->frame_MVV1->SetNo(1);
    ui->frame_MVV2->SetNo(2);
}

FrameBT::~FrameBT()
{
    delete ui;
}
