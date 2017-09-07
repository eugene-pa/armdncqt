#ifndef DBGTU_H
#define DBGTU_H

#include <QDialog>

namespace Ui {
class DbgTu;
}

class DbgTu : public QDialog
{
    Q_OBJECT

public:
    explicit DbgTu(QWidget *parent = 0);
    ~DbgTu();

private slots:
    void on_pushButton_MTU_clicked();

    void on_pushButton_RPC_clicked();

    void on_pushButton_Ebl_clicked();

private:
    Ui::DbgTu *ui;
};

#endif // DBGTU_H
