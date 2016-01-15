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

    void ChangeStation(class Station *);

    virtual void paintEvent(QPaintEvent *);
    virtual void closeEvent(QCloseEvent *);
private slots:
    void on_checkBox_toggled(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_tsSelected (int no);

private:
    Ui::DlgTsInfo *ui;
    class Station * pSt;

    void timerEvent(QTimerEvent *event);
    void fillTable();
};

#endif // DLGTSINFO_H
