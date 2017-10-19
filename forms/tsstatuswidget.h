#ifndef TSSTATUSWIDGET_H
#define TSSTATUSWIDGET_H

#include <QWidget>
#include "../spr/station.h"
#include "../spr/ts.h"

class TsStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TsStatusWidget(QWidget *parent = 0);
    ~TsStatusWidget();

    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent *);

    void updateWidget (Station * pst, int p=1);             // обновить
    void setPage (int p) { page = p; }                      //

    void setNormal (bool s ) { normal = s; update(); }
    void setActualNode(int n) { actualNode = n; update(); }

signals:
    void tsSelected (int no);                               // выделен сигнал

public slots:

private:
    bool normal;
    int dxy;
    int page;
    void DrawGrid(QPainter *p);                             // отрисовка окантовки
    void DrawTs  (QPainter *p);                             // отрисовка ТС
    Station * st;                                          // указатель на класс станции
    int actualNode;                                         // актуальный узел матрицы
    void timerEvent(QTimerEvent *event);
};

#endif // TSSTATUSWIDGET_H
