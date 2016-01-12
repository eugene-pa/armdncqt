#include "QPainter"
#include "tsstatuswidget.h"

TsStatusWidget::TsStatusWidget(QWidget *parent) : QWidget(parent)
{

}

TsStatusWidget::~TsStatusWidget()
{

}

void TsStatusWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    DrawGrid(&p);
}

void TsStatusWidget::DrawGrid(QPainter *p)
{
    for (int i=0; i<=32; i++)
    {
        p->setPen(i%8 ? Qt::gray : Qt::black);
        p->drawLine(0,dxy*i,dxy*32,dxy*i);
        p->drawLine(dxy*i,0,dxy*i,dxy*32);
    }
}

void TsStatusWidget::Update (class Station * pst)
{
    pSt = pst;

}
