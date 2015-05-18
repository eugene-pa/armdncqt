#include <QDebug>
#include <math.h>
#include "shape.h"
#include "shapeset.h"


DShape::DShape(QString& src, ShapeSet* parent)
{
    source = src;
    set = parent;
    idObj = 0;                                              // N РЦ/стрелки/светофора
    idst = 0;                                               // номер станции
    st   = 0;                                               // справочник станции
    x1 = x2 = y1 = y2 = 0.0;
    width = 0;
    height = 0;
    subtype = 0;                                            // тип отрезка/откоса/стрелки/светофора
    blinking = false;                                       // примитив мигает
    visible  = true;                                        // отрисовывается по условиям
    selected = false;                                       // отрисовывается по условиям
}

DShape::~DShape()
{

}

void DShape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)
}

void DShape::MoveRel(short dx,short dy)                     // 2009.03.11. перемещение на заданное расстояние
{
    x1 += dx;
    y1 += dy;
    x2 += dx;
    y2 += dy;
}

void DShape::Draw(QPainter* painter)                        // отрисовка
{
    paint (painter, 0,0);
}

void DShape::setDimensions ()
{
    try
    {
        width  = fabs (x2 - x1);
        height = fabs (y2 - y1);
        rect.setLeft  (x1);
        rect.setTop   (y1);
        rect.setRight (x2);
        rect.setBottom(y2);
    }
    catch (...)
    {
        //Trace.WriteLine(ex.Message);
    }
}

void DShape::log (QString msg)
{
    if (set && set->logger())
        set->logger()->log(QString(msg));
    else
    {
        qDebug() << msg;
    }
}
