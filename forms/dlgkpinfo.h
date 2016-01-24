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

    void redraw(class Station * p = nullptr);

public slots:
    void changeStation(class Station *);                    // смена станции

private:
    Ui::DlgKPinfo *ui;
    class Station * st;
    QColor okColor;
};

#endif // DLGKPINFO_H
