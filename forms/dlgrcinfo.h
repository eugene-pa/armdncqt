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
    explicit DlgRcInfo(QWidget *parent = 0);
    ~DlgRcInfo();

private:
    Ui::DlgRcInfo *ui;
};

#endif // DLGRCINFO_H
