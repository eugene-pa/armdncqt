#ifndef DLGRCINFO_H
#define DLGRCINFO_H

#include <QDialog>

namespace Ui {
class DlgRcInfo;
}

class DlgRcInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRcInfo(class Station * st, QWidget *parent = 0);
    ~DlgRcInfo();

public slots:
    void changeStation(class Station *);                    // смена станции
private slots:


private:
    virtual void closeEvent(QCloseEvent *);
    virtual void timerEvent(QTimerEvent *event);

    Ui::DlgRcInfo *ui;
    class Station * st;
    void fillData();
    void updateStatus();
    QBrush getBackground(class Rc * rc);

    static QBrush brushBusy;
    static QBrush brushBuzyZmk;


};

#endif // DLGRCINFO_H
