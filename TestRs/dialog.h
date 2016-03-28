#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "settingsdialog.h"

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
    void on_pushButton_Open_clicked();
    void applysettings();

private:
    Ui::Dialog *ui;
    SettingsDialog * settingdlg;
    QSerialPort *serial;
};

#endif // DIALOG_H
