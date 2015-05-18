#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QMainWindow>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>

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
    void on_actionNewForm_triggered();

private:
    Ui::MainWindow *ui;
    class QMdiArea *mdiArea;                                // виджет MDI
};

#endif // MAINWINDOW_H
