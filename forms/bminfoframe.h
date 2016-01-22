#ifndef BMINFOFRAME_H
#define BMINFOFRAME_H

#include <QFrame>
#include "../spr/station.h"

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

private:
    Ui::BmInfoFrame *ui;

    bool rsrv;                                              // признак резервного БМ
    class Station * st;                                     //
    class SysInfo * sysinfo;
    void redraw();

    QVector<class QLabel*> mt;
};

#endif // BMINFOFRAME_H
