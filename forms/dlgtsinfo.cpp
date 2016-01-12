#include "QPainter"

#include "dlgtsinfo.h"
#include "tsstatuswidget.h"
#include "ui_dlgtsinfo.h"
#include "../spr/station.h"

DlgTsInfo::DlgTsInfo(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgTsInfo)
{
    ui->setupUi(this);
    ui->labelSt->setText(pst->Name());

    ui->widgetTs->Update (pSt = pst);
}

DlgTsInfo::~DlgTsInfo()
{
    delete ui;
}

void DlgTsInfo::paintEvent(QPaintEvent *)
{
//    QPainter p(this);                       // Создаём новый объект рисовальщика
//    p.setPen(QPen(Qt::red,1,Qt::SolidLine));    // Настройки рисования
//    p.drawLine(0,0,100,100);           // Рисование линии
}
