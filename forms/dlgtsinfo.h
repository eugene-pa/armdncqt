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

private slots:
    void on_checkBox_toggled(bool checked);

private:
    Ui::DlgTsInfo *ui;
    class Station * pSt;

    void timerEvent(QTimerEvent *event);
    void fillTable();
};

#endif // DLGTSINFO_H
