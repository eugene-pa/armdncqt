#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    QString instr;
    //class RsProxy * rs;
    class RsAsinc * rs;
    virtual void timerEvent(QTimerEvent *event) override;

    Ui::Dialog *ui;
};

#endif // DIALOG_H
