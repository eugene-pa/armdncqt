#include "qled.h"


QLed::QLed(QWidget * p, ledShape shape, ledStatus status,  QBrush fore, QBrush back) : QLabel(p)
{    
    timerId = 0;
    set(shape, status, fore, back);
}

void QLed::set (ledShape shape, ledStatus status, QBrush fore, QBrush back)
{
    textColor    = QColor(64,64,64);                       // цвет текста
    borderColor  = QColor(Qt::gray);                       // цвет окантовки
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
    update();
}

void QLed::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    pulse = !pulse;
    if (isVisible())
        update();
}

void QLed::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)

    QPainter p(this);
    //p.setRenderHint(QPainter::Antialiasing);
    QBrush brush = status== blink ? pulse ? fore : back :
                   status==off ? back : fore;
    QRect rect = this->rect();
    rect.setBottomRight(QPoint(rect.right()-1,rect.bottom()-1));
    p.setPen(borderColor);
    p.setBrush(brush);
    if (shape == box)
        p.drawRect(rect);
    else
        p.drawEllipse(rect);
    p.setPen(Qt::black);
    if (text.length())
    {
        rect.setTopLeft(QPoint(rect.left()+1,rect.top()+1));
        p.setPen(textColor);
        p.setFont(QFont("Arial",-1, QFont::Bold));
        p.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
}

void QLed::mousePressEvent(QMouseEvent * event)
{
    Q_UNUSED(event)
    emit ledClicked(this);
}

void QLed::mouseMoveEvent(QMouseEvent *event)
{
    //QPoint point = event->pos();
    QToolTip::showText(event->globalPos(), objectName());
}
