#ifndef SHAPETRAIN_H
#define SHAPETRAIN_H

#include "shape.h"

class ShapeTrain : public DShape
{
public:
    ShapeTrain(ShapeSet* parent);
    ~ShapeTrain();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    virtual void  Draw (QPainter*);                         // функция рисования


    static bool bShowTrains;
    static bool bShowNonregTrains;

};

#endif // SHAPETRAIN_H
