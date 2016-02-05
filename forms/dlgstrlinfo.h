#ifndef DLGSTRLINFO_H
#define DLGSTRLINFO_H

#include <QDialog>

namespace Ui {
class DlgStrlInfo;
}

class DlgStrlInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStrlInfo(class Station * st, QWidget *parent = 0);
    ~DlgStrlInfo();

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void closeEvent(QCloseEvent *);

private:
    Ui::DlgStrlInfo *ui;

    class Station * st;
    void fillData();
    void UpdateStatus();
};

#endif // DLGSTRLINFO_H
