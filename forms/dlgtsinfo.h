#ifndef DLGTSINFO_H
#define DLGTSINFO_H

#include <QDialog>

namespace Ui {
class DlgTsInfo;
}

class DlgTsInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTsInfo(QWidget *parent = 0, class Station * pst = nullptr);
    ~DlgTsInfo();

    void paintEvent(QPaintEvent *);

private:
    Ui::DlgTsInfo *ui;
    class Station * pSt;
};

#endif // DLGTSINFO_H
