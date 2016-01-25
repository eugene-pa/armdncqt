#include "QPainter"
#include "qled.h"


QLed::QLed(QWidget * p, ledShape shape, ledStatus status,  QBrush fore, QBrush back) : QLabel(p)
{    
    timerId = 0;
    set(shape, status, fore, back);
}

void QLed::set (ledShape shape, ledStatus status, QBrush fore, QBrush back)
{
    this->shape  = shape;
    this->fore   = fore;
    this->back   = back;
    this->status = status;

    setVisible(status!=hidden);

    if (status != blink)
    {
        if (timerId)
        {
            killTimer(timerId);
            timerId = 0;
        }
    }
    else
    if (!timerId)
        timerId = startTimer(500);
}

void QLed::timerEvent(QTimerEvent *event)
{
    pulse = !pulse;
    update();
}

void QLed::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    //p.setRenderHint(QPainter::Antialiasing);
    QBrush brush = status== blink ? pulse ? fore : back :
                   status==off ? back : fore;
    QRect rect = this->rect();
    rect.setBottomRight(QPoint(rect.right()-1,rect.bottom()-1));
    p.setPen(QPen(brush.color()));
    p.setBrush(brush);
    if (shape == box)
        p.drawRect(rect);
    else
        p.drawEllipse(rect);
}



