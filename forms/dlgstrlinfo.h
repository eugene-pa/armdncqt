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

private:
    Ui::DlgStrlInfo *ui;

    class Station * st;
    void FillData();

};

#endif // DLGSTRLINFO_H
