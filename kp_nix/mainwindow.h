#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <thread>
#include <mutex>
#include <QMainWindow>
#include "../common/pamessage.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static MainWindow * mainWnd;

signals:
    void SendMsg(PaMessage *);

public slots:
    void GetMsg (PaMessage *);

private:
    Ui::MainWindow *ui;
    std::wstring config;

    std::shared_ptr<std::thread> pThreadPolling;                // smart-указатель на поток опроса динии связи
};

#endif // MAINWINDOW_H
