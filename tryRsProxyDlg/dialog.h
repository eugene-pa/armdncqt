#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <stdio.h>
#include <cstdio>
#include <iostream>											// std::cout
#include <sstream>
#include <thread>											// std::thread
#include <mutex>											// мьютексы
#include <queue>											// FIFO
#include <iomanip>
#include <../common/rsasinc.h>

namespace Ui
{
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
    void finished();
signals:
    void startrs();

private:
    QString instr;
    //class RsProxy * rs;
    //class RsAsinc * rs;
    std::thread * pThreadPolling;
    virtual void timerEvent(QTimerEvent *event) override;

    Ui::Dialog *ui;
};

#endif // DIALOG_H
