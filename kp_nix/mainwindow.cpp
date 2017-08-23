#include <mutex>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../common/common.h"

//#uising namespace std;

std::timed_mutex exit_lock;										// мьютекс, разрешающий завершение приложения

// Базовые потоки КП
void   ThreadTS				(long);							// функция потока опроса ТС
void   ThreadTU				(long);							// функция потока вывода ТУ
void   ThreadUpok			(long);							// функция потока обработки ОТУ УПОК+БРОК
void   ThreadSysCommand     (long);							// функция потока исполнения директив управления КП
void   ThreadPulse			(long);							// функция потока формирования программного пульса
void   ThreadMonitoring		(long);							// функция потока мониторинга состояния КП
void   ThreadTestTU			(long);							// функция потока циклического теста ТУ
void   ThreadWatchDog		(long);							// функция потока включения и управления сторожевым таймером
void   ThreadPolling		(long);							// функция потока опроса динии связи



MainWindow * MainWindow::mainWnd;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mainWnd = this;
    ui->setupUi(this);

    connect(this, SIGNAL(SendMsg(PaMessage*)), this, SLOT(GetMsg(PaMessage*)));

    exit_lock.lock();

    // создание рабочих потоков: создаем умные указатели, переопределяя действия деструктора
    // с помощью лямбда-функции, выполняющей ожидание завершения потока и удаление указателя
    config = L"COM3,38400,N,8,1";
//    pThreadPolling = std::shared_ptr<std::thread>(new std::thread(ThreadPolling, (long)&config),[](std::thread *p) { p->join();  Log(L"*Удаление pThreadPolling*"); delete p; });
    pThreadPolling = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling, (long)&config));
}

MainWindow::~MainWindow()
{
    exit_lock.unlock();
    //QThread::sleep(2);                                      // 3 секунды на обзор результатов
    // ожидание заершения потоков
//    if (pThreadPolling)
//    {
//        pThreadPolling->join();
//        delete pThreadPolling;
//    }
    delete ui;
}

// слот приема сообщения (работает в своем потоке)
// поэтому можно пользоваться элементами GUI
void MainWindow::GetMsg(PaMessage * pMsg)
{
    Log (pMsg->GetText());                                      // упрощеннно
    ui->statusBar->showMessage(QString::fromStdWString(pMsg->GetText()));
    delete pMsg;
}

// глобальная функция отправки сообщения главному окну
// генерирует сигнал MainWindow::SendMsg, что обеспечивает безопасную синхронизацию потоков
void SendMessage (PaMessage * pMsg)
{
    emit MainWindow::mainWnd->SendMsg(pMsg);
}

