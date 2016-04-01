#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingsdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Config_triggered();
    void applysettings();
    void on_action_Start_triggered();
    void on_action_Stop_triggered();

    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void timeout();                                         // сигнал-уведомление об отсутствии данных в канала данных
    void error  (int);                                      // сигнал-уведомление об ошибке
    void rsStarted();                                       // старт потока RS

    void closeEvent(QCloseEvent *event);

signals:
    void exit();

private:
    Ui::MainWindow *ui;
    class SettingsDialog * settingdlg;
    class RasRS * rasRs;

    QString GetHex(QByteArray& array, int maxlength);
};

#endif // MAINWINDOW_H
