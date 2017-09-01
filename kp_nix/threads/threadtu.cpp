// поток вывода ТУ

#include <string>
#include <thread>
#include "threadtu.h"
#include "../common/pamessage.h"
#include "../kp2017.h"
#include "../common/acksenum.h"

// Данный файл включает определения для класса Tu и код потока ThreadTU
// Класс ТУ создается при приеме команды в статической функции Tu::PushTu
// В этот момент создается умный разделяемый указатель, который записывается в очередь
// Tu:listToDo - статический член класса Tu
// Время жизни класса - до мосента удаления из очереди
// Учитывая организацию сплошного списка ТУ во фрейме ТУ, помещать ТУ в очередь исполненных большого резона нет
// кроме как отображения времени исполнения ТУ. Можно добавить столбец, а можно не добавлять,
// а при необходимости анализировать протокол (большого резона видеть на экране время исполнения
// последней ТУ не видно, хотя это вопрос обсуждаемый
// Если все-таки доваить еще одну очередь, время жизни будет определяться наличием ТУ хотя в одной из очередей

// статические члены класса
DWORD Tu::Counter = 0;
std::queue <std::shared_ptr<Tu>> Tu::listToDo;          // очередь ТУ на исполнение
std::mutex Tu::todo_lock;                               // блокировка доступа к очереди TODO

Tu::Tu(DWORD tu)
{
    id = ++Counter;
    this->tu = tu;

    // извлечь атрибуты ТУ из кода ТУ
    mod  = 1;                                           // модуль
    out  = id;                                          // выход (сейчас для имитации- порядковый номер ТУ)
    delay= 4000;                                        // задержка

    time (&tRcv);
    tBeg = tEnd = 0;                                    // нулевые времена косвенно определяют статус ТУ (начали исполнять, закончили)
}


void Tu::PushTu(DWORD _tu)                              // ТУ - в очередь
{
    std::shared_ptr<Tu> tu(new Tu(_tu), [](Tu *p) { Log (L"delete TU !!!"); delete p; } );
                                    //-------------------------------------------------  - так можно отследить факт удаления указателя,
                                    //                                                     можно убрать люмбда-функцию, включая запятую
    {
    std::lock_guard<std::mutex> locker(Tu::todo_lock);  // лучше освободить очередь до вызова SendMessage
    listToDo.push(tu);
    }
    SendMessage(new PaMessage(tuAckRcv, tu));
}


void ThreadTU (long)
{
    std::wstringstream s;
    s << L"Поток вывода ТУ запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

	// выборка и исполнение ТУ вплоть до запроса выхода (освобождение exit_lock)
	while (!exit_lock.try_lock_for(chronoMS(100)))
	{
        //SendMessage(new PaMessage(L"Ожидание ТУ"));
        std::shared_ptr<Tu> tu = nullptr;
		size_t n = 0;
		{
            std::lock_guard <std::mutex> locker(Tu::todo_lock);
            if ((n = Tu::listToDo.size()) > 0)
			{
                tu = Tu::listToDo.front();
                Tu::listToDo.pop();
			}
		}

        if (tu != nullptr)
		{
            SendMessage(new PaMessage(tuAckToDo, tu, s.str())); // принята к исполнению

            SleepMS(4000);										// задержка на исполнение ТУ

            SendMessage(new PaMessage(tuAckDone, tu, s.str())); // принята исполнена

		}
	}
	exit_lock.unlock();

    // очищаем очереди
    {                                                           // блок { } нужен для вызова деструктора lock_guard
    std::lock_guard <std::mutex> locker(Tu::todo_lock);
    while (Tu::listToDo.size() > 0)
        Tu::listToDo.pop();
    }

    s.str(std::wstring());
    s << L"Поток вывода ТУ завершен. threadid=" << std::this_thread::get_id();
    Log(s.str());                        // отправка SendMessage здесь уже не проходит, так как запущен деструктор главного окна
}


int Tu::todoSize()
{
    std::lock_guard <std::mutex> locker(todo_lock);
    return listToDo.size();
}

