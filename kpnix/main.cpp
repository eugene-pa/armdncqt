#include <QCoreApplication>
#include "main.h"

mutex con_lock;													// блокировка доступа к консоли
timed_mutex exit_lock;											// блокировка до выхода
bool  rqExit = false;											// запрос выхода
bool WaitThreadsPending();										// ожидание завершения потоков



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextCodec::setCodecForLocale( QTextCodec::codecForName("CP866"));
    QString s = "АБВГД";
    qDebug() << s.toStdString().c_str();
    cout << s.toStdString();

    //setlocale(LC_CTYPE, "rus"); // вызов функции настройки локали

#ifdef Q_OS_WIN
    //char * local = setlocale(LC_CTYPE, "utf-8");					// вызов функции настройки локали
    //char * local = setlocale(LC_ALL, "Russian");
    //if (local != NULL)
    //    cout << local << "\n";
#else // Q_OS_WIN
    set_keypress();
#endif

    exit_lock.lock ();											// блокируем мьютекс завершения

    // создание потоков
    pThreadTs			= new thread ( ThreadTS			, 0);	// поток опроса ТС
    pThreadTu			= new thread ( ThreadTU			, 0);	// поток вывода ТУ
    pThreadUpok			= new thread ( ThreadUpok		, 0);	// поток отработки ОТУ УПОК+БРОК
    pThreadMonitoring	= new thread ( ThreadMonitoring	, 0);	// поток мониторинга состояния КП
    pThreadPulse		= new thread ( ThreadPulse		, 0);	// поток формирования программного пульса
    pThreadSysCommand	= new thread ( ThreadSysCommand	, 0);	// поток исполнения директив управления КП
    pThreadTestTU		= new thread ( ThreadTestTU		, 0);	// поток циклического теста ТУ
    pThreadWatchDog		= new thread ( ThreadWatchDog	, 0);	// поток поток включения и управления сторожевым таймером

// отладка **************************************************************************************************************
    PushTu(1);
    PushTu(2);
    PushTu(3);

    getchar();

    PushTu(4);

    getchar();
// отладка **************************************************************************************************************


    exit_lock.unlock();											// разблокируем мьютекс завершения

    WaitThreadsPending();

    return a.exec();
}

bool WaitThreadsPending()
{
    // ожидание завершения потоков
    threadsafecout("Ожидаем завершения потоков и освобождаем ресурсы");
    if (pThreadTs)
    {
        pThreadTs->join();
        delete pThreadTs;
    }
    if (pThreadTu)
    {
        pThreadTu->join();
        delete pThreadTu;
    }
    if (pThreadUpok)
    {
        pThreadUpok->join();
        delete pThreadUpok;
    }
    if (pThreadMonitoring)
    {
        pThreadMonitoring->join();
        delete pThreadMonitoring;
    }
    if (pThreadPulse)
    {
        pThreadPulse->join();
        delete pThreadPulse;
    }

    if (pThreadSysCommand)
    {
        pThreadSysCommand->join();
        delete   pThreadSysCommand;
    }

    if (pThreadTestTU)
    {
        pThreadTestTU->join();
        delete pThreadTestTU;
    }
    if (pThreadWatchDog)
    {
        pThreadWatchDog->join();
        delete pThreadWatchDog;
    }

    return true;
}

// безопасный вывод на консоль с блокировкой мьютекса
void threadsafecout(const char * p)
{
    lock_guard<mutex> locker(con_lock);
    thread::id id = this_thread::get_id();
    cout << p << "(id=" << id << ")\n";
}

// безопасный вывод на консоль с блокировкой потока
void threadsafecout(string msg)
{
    lock_guard<mutex> locker(con_lock);
    cout << msg << "\n";
}

#ifndef Q_OS_WIN
#include <termios.h>
// танцы с бубном для эмуляции поведения _getch(), дающей ввод символа без подтверждения Enter
// (перепрограммируем режим терминала)
static struct termios stored_settings;

void set_keypress(void)
{
 struct termios new_settings;

 tcgetattr(0,&stored_settings);

 new_settings = stored_settings;

 /* Disable canonical mode, and set buffer size to 1 byte */
 new_settings.c_lflag &= (~ICANON);
 new_settings.c_cc[VTIME] = 0;
 new_settings.c_cc[VMIN] = 1;

 tcsetattr(0,TCSANOW,&new_settings);
 return;
}

void reset_keypress(void)
{
 tcsetattr(0,TCSANOW,&stored_settings);
 return;
}
#endif // #ifndef Q_OS_WIN
