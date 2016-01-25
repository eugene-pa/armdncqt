#ifndef BMINFOFRAME_H
#define BMINFOFRAME_H

#include <QFrame>
//#include <QBrush>
#include "../spr/station.h"
#include "qled.h"

namespace Ui {
class BmInfoFrame;
}

class BmInfoFrame : public QFrame
{
    Q_OBJECT

public:
    explicit BmInfoFrame(QWidget *parent = 0);
    ~BmInfoFrame();

    void setObj(class Station * p, bool s = false);         // назначить объект

    void timerEvent(QTimerEvent *event);

public slots:
    void ledClicked (QLed *);                               // уведомление о клике

private:
    Ui::BmInfoFrame *ui;

    bool rsrv;                                              // признак резервного БМ
    class Station * st;                                     //
    class SysInfo * sysinfo;

    void redraw();                                          // отрисовка состояния модуля
    void RedrawUndefined();                                 // отрисовка неопределенного состояния модуля (нет данных)

    QColor okColor;
    QColor erColor;
    QBrush okBrush;
    QBrush erBrush;
    QVector<class QLed*> mt;
};

#endif // BMINFOFRAME_H
