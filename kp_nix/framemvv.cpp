#include <bitset>
#include "framemvv.h"
#include "ui_framemvv.h"

// Панель состояния модулей одного МВВ 8 * 3 = 24 модуля

extern std::bitset<48> tsModules;                                      // модули ТС (битовый массив)
extern std::bitset<48> tuModules;                                      // модули ТУ (битовый массив)


FrameMVV::FrameMVV(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameMVV)
{
    ui->setupUi(this);

    // инициализируем указатели для дальнейшего использования по индексу
    modules[0]  = ui->label_M1;
    modules[1]  = ui->label_M2;
    modules[2]  = ui->label_M3;
    modules[3]  = ui->label_M4;
    modules[4]  = ui->label_M5;
    modules[5]  = ui->label_M6;
    modules[6]  = ui->label_M7;
    modules[7]  = ui->label_M8;
    modules[8]  = ui->label_M9;
    modules[9]  = ui->label_M10;
    modules[10] = ui->label_M11;
    modules[11] = ui->label_M12;
    modules[12] = ui->label_M13;
    modules[13] = ui->label_M14;
    modules[14] = ui->label_M15;
    modules[15] = ui->label_M16;
    modules[16] = ui->label_M17;
    modules[17] = ui->label_M18;
    modules[18] = ui->label_M19;
    modules[19] = ui->label_M20;
    modules[20] = ui->label_M21;
    modules[21] = ui->label_M22;
    modules[22] = ui->label_M23;
    modules[23] = ui->label_M24;
}

FrameMVV::~FrameMVV()
{
    delete ui;
}

void FrameMVV::SetNo(int n)
{
    no = n;

    // инициализация подписей с учетом номера МВВ
    ui->label_MVV       ->setText(QString("МВВ%1").arg(n));
    ui->label_BT1       ->setText(QString("БТ%1").arg(1+(n-1)*3));
    ui->label_BT2       ->setText(QString("БТ%1").arg(2+(n-1)*3));
    ui->label_BT3       ->setText(QString("БТ%1").arg(3+(n-1)*3));
    ui->label_BT1_beg   ->setText(QString("%1").arg(1 +(n-1)*24));
    ui->label_BT2_beg   ->setText(QString("%1").arg(9 +(n-1)*24));
    ui->label_BT3_beg   ->setText(QString("%1").arg(17+(n-1)*24));

    // инициализация должна выполняться с учетом номера МВВ и реальных модулей
    for (int i=0; i< 24; i++)
    {
        int indx = (no-1) * 24 + i;                         // индекс модуля в битовом массиве
        bool present = tsModules[indx] | tuModules[indx];
        modules[i]->set (QLed::ledShape::box  , present ? QLed::ledStatus::on : QLed::ledStatus::off, Qt::green);
        modules[i]->setTextColor(Qt::black);
        if (tuModules[indx])
            modules[i]->setText("У");                       // если ТУ - подписываем
    }
/*
    // пример отображения ошибки
    if (no==1)
        modules[1]->set (QLed::ledShape::box  , QLed::ledStatus::blink, Qt::red, Qt::yellow);
    if (no==2)
        modules[3]->set (QLed::ledShape::box  , QLed::ledStatus::blink, Qt::red, Qt::yellow);
*/
}
