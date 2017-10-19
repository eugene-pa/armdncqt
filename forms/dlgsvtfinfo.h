#ifndef DLGSVTFINFO_H
#define DLGSVTFINFO_H

#include <QDialog>

namespace Ui {
class DlgSvtfInfo;
}

class DlgSvtfInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSvtfInfo(class Station * st, QWidget *parent = 0);
    ~DlgSvtfInfo();

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void closeEvent(QCloseEvent *);

public slots:
    void changeStation(class Station *);                    // смена станции

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DlgSvtfInfo *ui;

    class Station * st;
    void fillData();
    void UpdateStatus();

};

#endif // DLGSVTFINFO_H
