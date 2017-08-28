#include <mutex>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common/common.h"
#include "common/acksenum.h"
#include "common/pamessage.h"
#include "threads/threadtu.h"

std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения

// Прототипы функций рабочих потоков ПО КП
void   ThreadPolling		(long);							// функция потока опроса динии связи
void   ThreadMonitoring		(long);							// функция потока мониторинга состояния КП
void   ThreadPulse			(long);							// функция потока формирования программного пульса
void   ThreadSysCommand     (long);							// функция потока исполнения директив управления КП
void   ThreadTestTU			(long);							// функция потока циклического теста ТУ
void   ThreadTS				(long);							// функция потока опроса ТС
void   ThreadTU				(long);							// функция потока вывода ТУ
void   ThreadUpok			(long);							// функция потока обработки ОТУ УПОК+БРОК
void   ThreadWatchDog		(long);							// функция потока включения и управления сторожевым таймером

MainWindow * MainWindow::mainWnd;                           // экземпляр главного окна

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mainWnd = this;
    ui->setupUi(this);

    Log(L"Запуск");

    // подключаем сигнал SendMsg, посылаемый из глобальной статической функции SendMessage, к слоту MainWindow::GetMsg
    connect(this, SIGNAL(SendMsg(PaMessage*)), this, SLOT(GetMsg(PaMessage*)));

    exit_lock.lock();

    // Создание рабочих потоков: создаем умные указатели unique_ptr, переопределяя действия деструктора
    // с помощью функционального объекта ThreadTerminater, выполняющего ожидание завершения потока,
    // необязательную запись в лог и удаление указателя
    //    ВАЖНО: если передавать пармаетр, например, строку конфигурации, то нельзя объявлять строку
    //           как локальный параметр на стеке, так как она будет использоваться в рабочем потоке позже,
    //           скорее всего, когда данная функция завершит работу и объект будет разрушен
    config = L"COM3,38400,N,8,1";
    pThreadPolling    = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPolling   , (long)&config));
    pThreadMonitoring = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadMonitoring, 0));
    pThreadPulse      = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadPulse     , 0));
    pThreadSysCommand = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadSysCommand, 0));
    pThreadTestTU     = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadTestTU    , 0));
    pThreadTs         = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadTS        , 0));
    pThreadTu         = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadTU        , 0));
    pThreadUpok       = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadUpok      , 0));
    pThreadWatchDog   = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadWatchDog  , 0));

#ifdef DBG_INCLUDE
    DBG_PushTu(111);
    DBG_PushTu(222);
    DBG_PushTu(333);
#endif // #ifdef DBG_INCLUDE

    //this->layout()->setMargin(2);
    ui->frameBase->layout()->setMargin(4);
}

MainWindow::~MainWindow()
{
    exit_lock.unlock();                                 // освобождаем мьютекс завершения
    // ВАЖНО: ожидание заершения потоков делать не надо - делается автоматически в делитерах смарт-указателя
    delete ui;
}

// слот приема сообщения (работает в основном потоке) поэтому можно пользоваться элементами GUI!
// здесь инициируется вся обработка окна GUI по уведомлениям от рабочих потоков
void MainWindow::GetMsg(PaMessage * pMsg)
{
    std::wstringstream tmp;
    // обработка сообщений отличается в зависимости от типа сообщения, ниже - тривиальная реализация
    switch (pMsg->GetAction())
    {
        case PaMessage::eventTrace:
        case PaMessage::eventReceive:
            Log (pMsg->GetText());                                                  // лог
            ui->statusBar->showMessage(QString::fromStdWString(pMsg->GetText()));   // GUI - строка состояния окна
            break;

        case PaMessage::eventTu:
            tmp << L"Команда ТУ " << pMsg->GetTu() << (pMsg->GetAck() == tuAckRcv    ?  L" принята" :
                                                       pMsg->GetAck() == tuAckToDo   ?  L" принята к исполнению" :
                                                       pMsg->GetAck() == tuAckIgnore ?  L" отвергнута" :
                                                       pMsg->GetAck() == tuAckDone   ?  L" исполнена" : L"")
                                                   << L".  В очереди " << todoSize() << L" ТУ";
            Log (tmp.str());                                                            // лог
            ui->statusBar->showMessage(QString::fromStdWString(tmp.str()));             // GUI - строка состояния окна
            break;
        default:
            break;
    }

    // удаляем сообщения явно (можно было бы использовать shared_ptr, но это утяжеляет синтаксис объявлений)
    delete pMsg;

    // ВАЖНО: при использовании указателя на доп. данные PaMessage::data необходимо аккуратно реализовать
    // стратегию использования данных (выделения и освобождения ресурсов)
}

// глобальная функция отправки сообщения главному окну
// вызывается из рабочих потоков и работает в рабочих потоках
// генерирует сигнал MainWindow::SendMsg, QT обеспечивает обработку сигнала в основном потоке
std::mutex sendMutex;
void SendMessage (PaMessage * pMsg)
{
    std::lock_guard <std::mutex> locker(sendMutex);
    emit MainWindow::mainWnd->SendMsg(pMsg);
}

