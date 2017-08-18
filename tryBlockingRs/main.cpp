#include <QCoreApplication>
#include <iostream>                                         // std::cout
#include <mutex>                                            // std::mutex
#include <thread>
#include <../common/blockingrs.h>
#include <../common/common.h>

#ifdef Q_OS_WIN
#include <io.h>
#include <fcntl.h>
#endif // #ifdef Q_OS_WIN

std::mutex con_lock;										// блокировка доступа к консоли

std::thread * pThreadPolling;                               // указатель на поток опроса линии связи
void ThreadPolling(long param);
std::timed_mutex exit_lock;                                 // блокировка до выхода

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef Q_OS_WIN
    // настраиваем консоль Windows на юникод
    // под LINUX строки юникод отображаются на консоли нормально без всяких танцев!
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
#endif  // #ifdef Q_OS_WIN

    exit_lock.lock ();										// блокируем мьютекс завершения (ждем освобождения во всех потоках)

    std::wstring config = L"COM4,38400,N,8,1";
    pThreadPolling = new std::thread ( ThreadPolling	, (long)&config);	// поток опроса линни связи

    getchar();
    Log (L"Процесс завершения работы. Ожидание завершения потоков...");
    exit_lock.unlock();

    QThread::sleep(2);                                      // 3 секунды на обзор результатов
    a.quit();                                               // return QCoreApplication::exec();
    return 0;
}



// безопасный (с блокировкой мьютекса) вывод строки на консоль
void threadsafecout(std::wstring msg)
{
    std::lock_guard<std::mutex> locker(con_lock);
    std::wcout << msg << std::endl << std::flush;
}

void Log(std::wstring msg)
{
    threadsafecout(msg);
}
