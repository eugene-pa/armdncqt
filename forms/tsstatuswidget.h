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

    void Update (Station * pst, int p=1);                   // обновить
    void setPage (int p) { page = p; }                      //

signals:

public slots:

private:
    const int dxy = 16;
    int page;
    void DrawGrid(QPainter *p);
    void DrawTs  (QPainter *p);
    Station * pSt;                                        // указатель на класс станции

};

#endif // TSSTATUSWIDGET_H
