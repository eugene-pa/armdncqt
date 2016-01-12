#ifndef TSSTATUSWIDGET_H
#define TSSTATUSWIDGET_H

#include <QWidget>

class TsStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TsStatusWidget(QWidget *parent = 0);
    ~TsStatusWidget();

    virtual void paintEvent(QPaintEvent* e);

    void Update (class Station * pst);

signals:

public slots:

private:
    const int dxy = 16;
    void DrawGrid(QPainter *p);
    class Station * pSt;                                        // указатель на класс станции

};

#endif // TSSTATUSWIDGET_H
