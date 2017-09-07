// поток вывода ТУ

#include <string>
#include <thread>
#include "../common/tu.h"
#include "../common/pamessage.h"
#include "../kp2017.h"
#include "../common/acksenum.h"

extern std::queue <std::shared_ptr<Tu>> rpcToDo;        // очередь ТУ РПЦ на исполнение
extern std::mutex rpc_lock;                             // блокировка доступа к очереди rpcToDo
void MoveTu (std::queue <std::shared_ptr<Tu>>& tmp);    // прототип функции перемещения ТУ в нужную очередь

// Данный файл включает код потока ThreadTU
// Учитывая организацию сплошного списка ТУ во фрейме ТУ, помещать ТУ в очередь исполненных большого резона нет
// Если все-таки доваить еще одну очередь, время жизни будет определяться наличием ТУ хотя в одной из очередей

// Основной поток выборки ТУ
void ThreadTU (long)
{
    std::wstringstream s;
    s << L"Поток вывода ТУ запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

    int delayMpcRpc = 0;                                        // суммарная задержка на ТУ МПЦ/РПЦ
	// выборка и исполнение ТУ вплоть до запроса выхода (освобождение exit_lock)
    while (!exit_lock.try_lock_for(chronoMS(100 + delayMpcRpc)))// с учетом delayMpcRpc
	{
        std::shared_ptr<Tu> tu = nullptr;                       // указатель на актуальную ТУ
        delayMpcRpc = 0;                                        // суммарная выдержка для ТУ МПЦ/РПЦ
        {                                                       // обрамляющий действие блокировки блок {}
            std::lock_guard <std::mutex> locker(Tu::todo_lock); // блокировка todo_lock
            if (Tu::listToDo.size() > 0)
			{
                tu = Tu::listToDo.front();                      // выбираем ТУ, анализируем ее тип
                SUBSYSTYPE subsys = tu->GetSys();               // тип подсистемы очередной команды

                // если не КП - извлекаем и помещаем в нужную очередь все, подряд идущие, ТУ данного типа
                if (subsys != SYS_KP)
                {
                    std::queue <std::shared_ptr<Tu>> tmp;       // временная буферная очередь
                    while (tu!=nullptr && tu->GetSys() == subsys)// извлекаем и перемещаем все ТУ для данной подсистемы
                    {
                        tmp.push(tu);                           // помещаем во временую очередь
                        delayMpcRpc += 1000;                    // суммируем задержку на ТУ МПЦ (обдумать алгоритм формирования значения)
                        Tu::listToDo.pop();                     // извлекаем ТУ из общей очереди
                        tu = Tu::listToDo.size() ? Tu::listToDo.front() : nullptr;// если не пусто, просматриваем следующую (без извлечения)
                    }
                    // все однотипные подряд идущие ТУ помещены во временную очередь, перемещаем
                    MoveTu(tmp);                                // перемещаем ТУ в нужную очередь по типу подсистемы
                    continue;                                   // тип подсистемы верхней ТУ в очереди сменился, продолжаем цикл выборки
                }                                               // todo_lock разблокируется при этом

                // Обычная ТУ. Извлекаем для исполнения
                Tu::listToDo.pop();
			}
        }                                                       // разблокировка todo_lock

        if (tu != nullptr)
		{
            SendMessage(new PaMessage(tuAckToDo, tu));          // принята к исполнению

            SleepMS(4000);										// задержка на исполнение ТУ

            SendMessage(new PaMessage(tuAckDone, tu));          // принята исполнена

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


// перемещение ТУ из временной очереди в нужную по типу подсистемы (ДОРАБОТАТЬ ДЛЯ ДРУГИХ МПЦ)
// можно параметризовать функцию, передав ей ссылку на целевую очередь и мьютекс
// тогда switch для определения параметров будет в вызывающей программе
void MoveTu (std::queue <std::shared_ptr<Tu>>& tmp)
{
    switch (tmp.front()->GetSys())                          // выбор по типу первой ТУ в очереди
    {
        case SYS_RPC:
            {
            std::lock_guard <std::mutex> locker(rpc_lock);  // блокируем очередь РПЦ
            do
            {
                rpcToDo.push(tmp.front());                  // помещаем в очередь РПЦ
                tmp.pop();                                  // удаляем из временной
            }
                while (tmp.size() > 0);                     // продолжаем пока очередь не пуста
            }                                               // разблокируем очередь РПЦ
            break;
        case SYS_EBL:
            break;
        case SYS_EM:
            break;
        case SYS_MZF:
            break;
        case SYS_MPK:
            break;
        default:
            break;
    }
}


