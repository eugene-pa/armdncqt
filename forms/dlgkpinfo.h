#ifndef DLGKPINFO_H
#define DLGKPINFO_H

#include <QDialog>

namespace Ui {
class DlgKPinfo;
}

class DlgKPinfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgKPinfo(QWidget *parent = 0);
    ~DlgKPinfo();

private:
    Ui::DlgKPinfo *ui;
};

#endif // DLGKPINFO_H
