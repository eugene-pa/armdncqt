#ifndef QLED_H
#define QLED_H

#include "QLabel"
#include "QColor"
#include "QBrush"


class QLed : public QLabel
{
public:
enum ledStatus
{
    on,
    off,
    hidden,
    blink,
};

enum ledShape
{
    round,
    box,
    tu,
};

    QLed(QWidget * p,ledShape shape=round, ledStatus status=on, QBrush fore=Qt::green, QBrush back = Qt::gray);
    void set (ledShape shape=round, ledStatus = on, QBrush fore=Qt::green, QBrush back = Qt::gray);
    void setText (QString value) { text = value; }

protected:
void timerEvent(QTimerEvent *event);
void paintEvent(QPaintEvent* e);

private:
    ledShape  shape;
    ledStatus status;
    int       timerId;
    bool      pulse;
    QBrush    fore;                                         // актуальный цвет
    QBrush    back;                                         // цвет фонапри мигании
    QString   text;
};

#endif // QLED_H
