// поток вывода ТУ

#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include "common/common.h"
#include "common/pamessage.h"
#include "threadts.h"
#include "../kp2017.h"

std::mutex todo_lock;                                           // блокировка доступа к очереди TODO/DONE
std::mutex done_lock;											// блокировка доступа к очереди DONE

std::queue <DWORD> listToDo;									// очередь ТУ на исполнение
std::queue <DWORD> listDone;									// очередь исполненных ТУ

void ThreadTU (long)
{
    std::wstringstream s;
    s << L"Поток вывода ТУ запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

	// выборка и исполнение ТУ вплоть до запроса выхода (освобождение exit_lock)
	while (!exit_lock.try_lock_for(chronoMS(100)))
	{
        SendMessage(new PaMessage(L"Ожидание ТУ"));
		DWORD tu = 0;
		size_t n = 0;
		{
            std::lock_guard <std::mutex> locker(todo_lock);
			if ((n = listToDo.size()) > 0)
			{
				tu = listToDo.front();
				listToDo.pop();
			}
		}

		if (tu)
		{
            std::wstring msg = L"\nВ очереди " + std::to_wstring(n) + L" ТУ.  Исполняем ТУ: " + std::to_wstring(tu);
            //threadsafecout(msg);
            //SendMessage(new PaMessage(s.str()));

			SleepMS(3000);										// задержка на исполнение ТУ

            msg = L"Исполнили ТУ: " + std::to_wstring(tu);
            //threadsafecout(msg);

            std::lock_guard <std::mutex> locker(done_lock);               // помещаем ТУ в очередь исполненных
            listDone.push(tu);
            if (listDone.size() > 16)                           // длина исполненных <=16
                listDone.pop();
		}
	}
	exit_lock.unlock();

    // очищаем очереди
    {                                                           // блок { } нужен для вызова деструктора lock_guard
    std::lock_guard <std::mutex> locker(todo_lock);
    while (listToDo.size() > 0)
        listToDo.pop();
    }
    {
    std::lock_guard <std::mutex> locker(done_lock);
    while (listDone.size() > 0)
        listDone.pop();
    }

    s.str(std::wstring());
    s << L"Поток вывода ТУ завершен. threadid=" << std::this_thread::get_id();
    Log(s.str());                        // отправка SendMessage здесь уже не проходит, так как запущен деструктор главного окна
}

void PushTu(DWORD tu)
{
    std::wstring msg;
    msg = L"\nСтавим в очередь ТУ" + std::to_wstring(tu);
    SendMessage(new PaMessage(msg));

	// lock_guard - STL класс С++, в конструкторе блокирует, в деструкторе разблокирует
    std::lock_guard<std::mutex> locker(todo_lock);
	listToDo.push(tu);
}
