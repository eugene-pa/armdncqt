#ifndef DLGPEREEZD_H
#define DLGPEREEZD_H

#include <QDialog>
#include "../spr/pereezd.h"

namespace Ui {
class DlgPereezd;
}

class DlgPereezd : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPereezd(QWidget *parent = 0);
    ~DlgPereezd();

private:
    // события
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void closeEvent(QCloseEvent *)  override;

    void updateStatus();
    Ui::DlgPereezd *ui;
};

#endif // DLGPEREEZD_H
