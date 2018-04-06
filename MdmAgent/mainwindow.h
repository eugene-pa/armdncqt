#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sstream>
#include <../common/defines.h>
#include <../common/logger.h>
#include <../common/blockingrs.h>

void Log (std::wstring);

namespace Ui {
class MainWindow;
}


// класс-deleter для завершения рабочих потоков; используется в смарт-указателях std::unique_ptr
// выполняет: ожидание завершения, вывод в лог и удаление указателя
class ThreadTerminater
{
public:
    void operator () (std::thread * p)
    {
        auto id = p->get_id();                                          // запоминаем id, пока поток живой
        p->join();                                                      // ожидаем завершения потока
        std::wstringstream s;
        s << L"Удаление указателя на поток " << id;                     // если хотим убедиться в удалении указателя
        Log(s.str());                                                   // выводим лог
        delete p;                                                       // удаляем указатель
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    static MainWindow * mainWnd;

signals:
    void SendMsg(int, void *);

public slots:
    void GetMsg (int, void *);

    void dataready(QByteArray);                                         // сигнал-уведомление о готовности данных
    void timeout();                                                     // сигнал-уведомление об отсутствии данных в канала данных
    void error  (int);                                                  // сигнал-уведомление об ошибке
    void rsStarted();                                                   // старт потока RS
    //void rsFinished();                                                // завершение потока RS

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


    std::wstring config;                                                // конфигурация порта
    std::unique_ptr<std::thread, ThreadTerminater> pThreadPolling;      // smart-указатель на поток опроса динии связи
    //class BlockingRS * rasRs;
};

#endif // MAINWINDOW_H
