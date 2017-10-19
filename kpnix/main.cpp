#include <QCoreApplication>
#include <QDebug>

#include "main.h"
#include "objmain.h"
#include "../common/rsasinc.h"

mutex con_lock;													// блокировка доступа к консоли
timed_mutex exit_lock;											// блокировка до выхода
bool  rqExit = false;											// запрос выхода
bool WaitThreadsPending();										// ожидание завершения потоков

RsAsinc * pRS;
PaSender paSender;                                              // один класс для отправки сообщений

ObjMain  MainObj;

// завершающая работу функция: освобождение мьютекса, ожидание завершения потоков, очистка
void Cleanup()
{
    exit_lock.unlock();											// разблокируем мьютекс завершения
    WaitThreadsPending();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef Q_OS_WIN
    // настраиваем консоль Windows на юникод
    // под LINUX строки юникод отображаются на консоли нормально без всяких танцев!
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin),  _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
#endif  // #ifdef Q_OS_WIN

    wcout << L"Старт" << endl;

    exit_lock.lock ();											// блокируем мьютекс завершения (ждем освобождения во всех потоках)

    // устанавливаем функтор отображения в потоке threadpolling
    extern std::function<void(PaSender&, class PaMessage *)> rsNotifier;
    rsNotifier = &PaSender::SendMsg;
    QObject::connect(&paSender, SIGNAL(SendMessage(PaMessage*)), &MainObj, SLOT(GetMessage(PaMessage*)));
    //QObject::connect(&a, SIGNAL(aboutToQuit()), &MainObj, SLOT(Quit()));

    // создание потоков
    pThreadTs			= new thread ( ThreadTS			, 0);	// поток опроса ТС
    pThreadTu			= new thread ( ThreadTU			, 0);	// поток вывода ТУ
    pThreadUpok			= new thread ( ThreadUpok		, 0);	// поток отработки ОТУ УПОК+БРОК
    pThreadMonitoring	= new thread ( ThreadMonitoring	, 0);	// поток мониторинга состояния КП
    pThreadPulse		= new thread ( ThreadPulse		, 0);	// поток формирования программного пульса
    pThreadSysCommand	= new thread ( ThreadSysCommand	, 0);	// поток исполнения директив управления КП
    pThreadTestTU		= new thread ( ThreadTestTU		, 0);	// поток циклического теста ТУ
    pThreadWatchDog		= new thread ( ThreadWatchDog	, 0);	// поток включения и управления сторожевым таймером

// отладка **************************************************************************************************************
    PushTu(1);
    PushTu(2);
    PushTu(3);
    PushTu(4);
// отладка **************************************************************************************************************

// Следующий блок, обрамленный комментарием "// -------- COM-порт" можно закомментировать, чтобы проверить механизм завершения и очистки
// Для работы COM-порта (точнее, механизма сигналов и слотов )нужен цикл обработки a.exec(),
// после которого я не нашел пути выполнения очистки, а создавать класс ради деструктора было лень
// -------- COM-порт ----------------------------------------------------------------------------------------------------
    // Важно: - в консольном приложении для работы сигналов и слотов класс RsAsinc должен быть создан в основном потоке
    //        - основной поток должен запуситить цикл обработки сообщений a.exec()
    pRS = new RsAsinc("COM3,38400,N,8,1");
    pThreadPolling		= new thread ( ThreadPolling	, (long)pRS);	// поток опроса линни связи

    // чтобы работал механизм сигналов и слотов, нужно запустить a.exec, однако при этом не получается выполнить
    // корректное завершение приложения с корректной очисткой (Cleanup)
    // Можно сделать обертку функуиональности main, производную от QObject и привязать к сигналу aboutToQuit:
    // QObject::connect(&app, SIGNAL(aboutToQuit()), &wrapper, SLOT(doSomething()));
    // Есть глоб.функция qAddPostRoutine, по идее устанавливающая функцию очистки, но до нее программа у меня никак не доходит.
    //qAddPostRoutine(Cleanup);                                       // этот механизм не срабатывает
    return a.exec();                                                // НУЖЕН ДЛЯ COM-порта
// -------- COM-порт ----------------------------------------------------------------------------------------------------

    Cleanup();
    return a.exec();
}


// ожидание завершения рабочих потоков
bool WaitThreadsPending()
{
    // ожидание завершения потоков (функция join() обеспечивает ожидание завершения потока)
    threadsafecout(L"Ожидаем завершения потоков и освобождаем ресурсы");

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
    if (pThreadPolling)
    {
        pThreadPolling->join();
        delete pThreadPolling;
    }

    return true;
}


// безопасный (с блокировкой мьютекса) вывод строки на консоль
void threadsafecout(wstring msg)
{
    lock_guard<mutex> locker(con_lock);

    wcout << msg << "\n";
}

void Log(std::wstring msg)
{
    threadsafecout(msg);
}

// безопасный (с блокировкой мьютекса) вывод строки символов на консоль с указанием потока
//void threadsafecout(const wchar_t * p)
//{
//    lock_guard<mutex> locker(con_lock);
//    thread::id id = this_thread::get_id();
//    wcout << p << L"(id=" << id << L")\n";
//}


// рекомендованные реализации для преобразования QString <-> wstring
std::wstring qToStdWString(const QString &str)
{
#ifdef _MSC_VER
 return std::wstring((const wchar_t*)str.utf16());          // MSVC
#else
 return str.toStdWString();                                 // GCC
#endif
}

QString stdWToQString(const std::wstring &str)
{
#ifdef _MSC_VER
 return QString::fromUtf16((const ushort*)str.c_str());     // MSVC
#else
 return QString::fromStdWString(str);                       // GCC
#endif
}

