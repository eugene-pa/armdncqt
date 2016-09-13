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
    Ui::DlgPeregonInfo *ui;
};

#endif // DLGPEREGONINFO_H
