#ifndef DLGKPINFO_H
#define DLGKPINFO_H

#include <QDialog>
#include "../spr/station.h"

namespace Ui {
class DlgKPinfo;
}

class DlgKPinfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgKPinfo(class Station * p, QWidget *parent = 0);
    ~DlgKPinfo();

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void paintEvent(QPaintEvent* e);

public slots:
    void changeStation(class Station *);                    // смена станции

private:
    void redraw(class Station * p = nullptr);
    Ui::DlgKPinfo *ui;
    class Station * st;
    QColor okColor;
};

#endif // DLGKPINFO_H
