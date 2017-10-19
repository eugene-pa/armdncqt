#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <../common/defines.h>
#include <../common/logger.h>

void Log (std::wstring);

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

private slots:
    void on_action_triggered();

    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void timeout();                                         // сигнал-уведомление об отсутствии данных в канала данных
    void error  (int);                                      // сигнал-уведомление об ошибке
    void rsStarted();                                       // старт потока RS
    //void rsFinished();                                      // завершение потока RS

    void on_pushButtonMainOff_clicked();

    void on_pushButtonRsrvOff_clicked();

    void on_pushButtonToMain_clicked();

    void on_pushButtonToRsrv_clicked();

    void on_pushButtonTest_clicked();

    void on_pushButtonATU_clicked();

    void on_pushButtonReset_clicked();

    void on_pushButtonGetReconnect_clicked();

    void on_pushButtonResetMain_clicked();

    void on_pushButtonResetRsrv_clicked();

    void on_pushButtonWatchdog_clicked();

signals:
    void exit();

private:
    Ui::MainWindow *ui;
    class RasRs * rasRs;
};

#endif // MAINWINDOW_H
