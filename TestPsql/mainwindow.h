#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_pushButtoWrite_clicked();

    void on_pushButtoUpdate_clicked();

private:
    Ui::MainWindow *ui;
    class Logger * logger;
    class SqlBlackBox * blackbox;

    void UpdateList();
};



#endif // MAINWINDOW_H
