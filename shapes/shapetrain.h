#ifndef SHAPETRAIN_H
#define SHAPETRAIN_H


class ShapeTrain : public DShape
{
public:
    ShapeTrain();
    ~ShapeTrain();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    virtual void  Draw (QPainter*);                         // функция рисования


};

#endif // SHAPETRAIN_H
