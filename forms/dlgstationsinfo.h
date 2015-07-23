#ifndef DLGSTATIONSINFO_H
#define DLGSTATIONSINFO_H

#include <QDialog>

namespace Ui {
class DlgStationsInfo;
}

class DlgStationsInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStationsInfo(QWidget *parent = 0);
    ~DlgStationsInfo();

private:
    Ui::DlgStationsInfo *ui;
};

#endif // DLGSTATIONSINFO_H
