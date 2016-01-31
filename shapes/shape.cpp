#include <QDebug>
#include <math.h>
#include "shape.h"
#include "colorscheme.h"
#include "shapeset.h"
#include "shaperc.h"
#include "shapetrnsp.h"

ColorScheme * DShape::colorScheme;                          // цветовые схемы
qreal	DShape::mThick = 4;                                 // толщина линий

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
    state = new Status();
}

DShape::~DShape()
{
    delete state;
}

// вызов инициализации статических инструментов отрисовки для асех примитивов
void DShape::InitInstruments(QString bdpath, Logger& logger)
{
    colorScheme = new ColorScheme(bdpath, &logger);         // загрузка цветовых схем
    TrnspDescription::readBd(bdpath, logger);               // загрузка транспарантов

    ShapeRc::InitInstruments();                             // инициализация инструментов примитивов
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
