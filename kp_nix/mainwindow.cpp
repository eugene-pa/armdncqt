#include <mutex>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common/common.h"
#include "common/acksenum.h"
#include "common/pamessage.h"
#include "threads/threadtu.h"
#include "jsonireader.h"

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
    Log(L"Чтение настроечного файла");

    const char * filename = "C:\\json\\iniJson_nix.txt";    // имя настроечного файла для примера (будет определяться переключателями по номеру участка)
    int nost = 2;                                           // номер станции для примера
    JsoniReader(filename, nost);                            // читаем настройки из настроечного файла для указанного адреса

    extern QString    krugName;                             // глоб.переменные должны быть объявлены
    extern QString    stName;                               // глоб.переменные должны быть объявлены
    // заголовок окна
    QString t = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss  ");
    this->setWindowTitle(QString("ДЦ ЮГ. Участок '%1'. Станция %2. Запуск %3 (%4)").arg(krugName).arg(stName).arg(t).arg(filename));

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

    ui->mainToolBar->setHidden(true);

    //this->layout()->setMargin(2);
    ui->frameBase->layout()->setMargin(4);

    setIconSize(QSize(32,20));

    // Инициализируем номера МВВ во фреймах
    ui->frameMVV1->SetNo(1);
    ui->frameMVV2->SetNo(2);

    // инициализируем классы фреймов пассивного и активного модемов
    // нужно привязать к фреймам классы, отвечающие за работу соответствующих каналов
    // ui->groupBox_ACT
    // ui->groupBox_PSV

#ifdef DBG_INCLUDE
    Tu::PushTu(111);
    Tu::PushTu(222);
    Tu::PushTu(333);
#endif // #ifdef DBG_INCLUDE

}

MainWindow::~MainWindow()
{
    exit_lock.unlock();                                 // освобождаем мьютекс завершения
    // ВАЖНО: ожидание заершения потоков делать не надо - делается автоматически в делитерах смарт-указателя
    delete ui;
}

// =============================== Обработчики опций меню =================================

// Обработчик "1. Заблокировать/разблокировать"
void MainWindow::on_action_Lock_triggered()
{

}

// Обработчик "2.Обновить"
void MainWindow::on_action_Refresh_triggered()
{

}

// Обработчик "3.ТС"
void MainWindow::on_action_TS_triggered()
{

}

// Обработчик "4.очистить"
void MainWindow::on_action_Clear_triggered()
{

}

// Обработчик "5.Настройки"
void MainWindow::on_action_Ini_triggered()
{

}

// Обработчик "6.Протокол"
void MainWindow::on_action_Log_triggered()
{

}

// Обработчик "7.ТУ"
void MainWindow::on_action_TU_triggered()
{
#ifdef DBG_INCLUDE
    Tu::PushTu(0);
#endif // #ifdef DBG_INCLUDE
}

// скрыть/показать тулбар
void MainWindow::on_action_Toolbar_triggered()
{
    ui->mainToolBar->setHidden(ui->mainToolBar->isHidden() ? false : true);
}

// =================================================================================================


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
            tmp << L"Команда ТУ " << pMsg->GetTu()->GetTu() << (pMsg->GetAck() == tuAckRcv    ?  L" принята" :
                                                                pMsg->GetAck() == tuAckToDo   ?  L" принята к исполнению" :
                                                                pMsg->GetAck() == tuAckIgnore ?  L" отвергнута" :
                                                                pMsg->GetAck() == tuAckDone   ?  L" исполнена" : L"")
                                                            << L".  В очереди " << Tu::todoSize() << L" ТУ";
            Log (tmp.str());                                                            // лог
            ui->statusBar->showMessage(QString::fromStdWString(tmp.str()));             // GUI - строка состояния окна

            ui->frameTU->UpdateQueues(pMsg);                                            // обновление очередей
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



