#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QDateTime>
#include <QMutex>
#include <QWaitCondition>
#include <QMainWindow>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void wait(int);

private slots:
    void slotTimeout1();
    void slotEvent1(int);
    void slotTimeout2();
    void slotEvent2(int);
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QThread workerThread1;                                  // рабочий поток 1
    QThread workerThread2;                                  // рабочий поток 2
    QWaitCondition * condition;                             // условие ожидания данных
    //QMutex watermutex;                                      // блокировка условия ожидания

};

#endif // MAINWINDOW_H
