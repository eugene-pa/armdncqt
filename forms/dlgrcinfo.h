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

private slots:


private:
    Ui::DlgRcInfo *ui;
    class Station * st;
    void FillData();

};

#endif // DLGRCINFO_H