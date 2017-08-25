#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <sstream>
#include <thread>
#include <mutex>
#include <QMainWindow>
#include "common/common.h"
//#include "common/pamessage.h"

namespace Ui
{
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
        s << L"Удаление указателя на поток " << id;                   // если хотим убедиться в удалении указателя
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
    static MainWindow * mainWnd;

signals:
    void SendMsg(class PaMessage *);

public slots:
    void GetMsg (class PaMessage *);

private:
    Ui::MainWindow *ui;
    std::wstring config;
    
    // умные указатели на рабочие потоки с заданием делитера, обеспечивающего ожидание завершения, вывод в лог и удаление указателя
    std::unique_ptr<std::thread, ThreadTerminater> pThreadPolling;      // smart-указатель на поток опроса динии связи
    std::unique_ptr<std::thread, ThreadTerminater> pThreadTs;			// указатель на поток опроса ТС
    std::unique_ptr<std::thread, ThreadTerminater> pThreadTu;			// указатель на поток вывода ТУ
    std::unique_ptr<std::thread, ThreadTerminater> pThreadUpok;			// указатель на поток обработки ОТУ УПОК+БРОК
    std::unique_ptr<std::thread, ThreadTerminater> pThreadSysCommand;	// указатель на поток исполнения директив управления КП
    std::unique_ptr<std::thread, ThreadTerminater> pThreadPulse;		// указатель на поток формирования программного пульса
    std::unique_ptr<std::thread, ThreadTerminater> pThreadMonitoring;	// указатель на поток мониторинга состояния КП
    std::unique_ptr<std::thread, ThreadTerminater> pThreadTestTU;		// указатель на поток циклического теста ТУ
    std::unique_ptr<std::thread, ThreadTerminater> pThreadWatchDog;		// указатель на поток включения и управления сторожевым таймером

};

#endif // MAINWINDOW_H
