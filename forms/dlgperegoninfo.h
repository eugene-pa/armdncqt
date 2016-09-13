#ifndef DLGPEREGONINFO_H
#define DLGPEREGONINFO_H

#include <QDialog>

namespace Ui {
class DlgPeregonInfo;
}

class DlgPeregonInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPeregonInfo(QWidget *parent = 0);
    ~DlgPeregonInfo();

private:
    QString s;

    // события
    virtual void timerEvent(QTimerEvent *event) override;
    virtual void closeEvent(QCloseEvent *)  override;
    void updateStatus();
    Ui::DlgPeregonInfo *ui;
};

#endif // DLGPEREGONINFO_H
