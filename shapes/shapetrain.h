#ifndef SHAPETRAIN_H
#define SHAPETRAIN_H

#include "shape.h"

class ShapeTrain : public DShape
{
public:
    ShapeTrain(ShapeSet* parent);
    ~ShapeTrain();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
    virtual void  Draw (QPainter*) override;                // функция рисования
    virtual QRectF boundingRect() const override;

    static bool bShowTrains;
    static bool bShowNonregTrains;

    static QBrush GetBrush(int no);
};

#endif // SHAPETRAIN_H
