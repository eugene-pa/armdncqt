// поток вывода ТУ

#include "main.h"

thread * pThreadTu;												// указатель на поток вывода ТУ

mutex todo_lock;												// блокировка доступа к очереди TODO/DONE
mutex done_lock;												// блокировка доступа к очереди DONE

queue <DWORD> listToDo;											// очередь ТУ на исполнение
queue <DWORD> listDone;											// очередь исполненных ТУ


void ThreadTU (long)
{
	threadsafecout("Поток TU запущен!");

	// выборка и исполнение ТУ вплоть до запроса выхода (освобождение exit_lock)
	while (!exit_lock.try_lock_for(chronoMS(100)))
	{
		threadsafecout("Ожидание ТУ");
		DWORD tu = 0;
		size_t n = 0;
		{
			lock_guard <mutex> locker(todo_lock);
			if ((n = listToDo.size()) > 0)
			{
				tu = listToDo.front();
				listToDo.pop();
			}
		}

		if (tu)
		{
            string msg = "\nВ очереди " + to_string(n) + " ТУ.  Исполняем ТУ: ";
            threadsafecout(msg);

			SleepMS(3000);										// задержка на исполнение ТУ

            msg = "Исполнили ТУ: " + to_string(tu);
            threadsafecout(msg);
		}
	}
	exit_lock.unlock();

    // очищаем очереди
    {
    lock_guard <mutex> locker(todo_lock);
    while (listToDo.size() > 0)
        listToDo.pop();
    }
    {
	lock_guard <mutex> locker(done_lock);
    while (listDone.size() > 0)
        listDone.pop();
    }
	threadsafecout("Поток TU завершен!");

}

void PushTu(DWORD tu)
{
    string msg;
    msg = "\nСтавим в очередь ТУ" + to_string(tu);
    threadsafecout(msg);

	// lock_guard - STL класс С++, в конструкторе блокирует, в деструкторе разблокирует
	lock_guard<mutex> locker(todo_lock);
	listToDo.push(tu);
}
