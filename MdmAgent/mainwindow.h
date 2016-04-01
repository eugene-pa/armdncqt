#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <../common/defines.h>
#include <../common/logger.h>

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
    void on_action_triggered();

    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void timeout();                                         // сигнал-уведомление об отсутствии данных в канала данных
    void error  (int);                                      // сигнал-уведомление об ошибке
    void rsStarted();                                       // старт потока RS
    //void rsFinished();                                      // завершение потока RS

    void on_MainWindow_destroyed();

signals:
    void exit();

private:
    Ui::MainWindow *ui;
    class RasRs * rasRs;
};

#endif // MAINWINDOW_H
