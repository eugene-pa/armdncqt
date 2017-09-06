// поток вывода ТУ

#include <string>
#include <thread>
#include "threadtu.h"
#include "../common/pamessage.h"
#include "../kp2017.h"
#include "../common/acksenum.h"

extern std::queue <std::shared_ptr<Tu>> rpcToDo;        // очередь ТУ РПЦ на исполнение
extern std::mutex rpc_lock;                             // блокировка доступа к очереди rpcToDo
void MoveTu (std::queue <std::shared_ptr<Tu>>& tmp);    // прототип функции перемещения ТУ в нужную очередь

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


// Класс Tu -----------------------------------------------------------------------------------------------------------------------------------
// статические члены класса
DWORD Tu::Counter = 0;                                  // уникальный счетчик в пределах сессии
std::queue <std::shared_ptr<Tu>> Tu::listToDo;          // очередь ТУ на исполнение
std::mutex Tu::todo_lock;                               // блокировка доступа к очереди TODO

Tu::Tu(WORD tu, SUBSYSTYPE sys/*=SYS_KP*/, ARM_TYPE arm/* = ARM_DNC*/, CANAL_TYPE ch/* = CANAL_1*/)
{
    id = ++Counter;                                     // уникальный счетчик в пределах сессии
    this->tu = tu;                                      // код ТУ

    sysType   = sys;                                    // тип ЭЦ, на который направляется ТУ
    armType   = arm;                                    // тип АРМ, выдавший ТУ
    canalType = ch;                                     // тип канала (используются для идентификации канала ТУ ЭЦ-МПК)

    time (&tRcv);
    tBeg = tEnd = 0;                                    // нулевые времена косвенно определяют статус ТУ (начали исполнять, закончили)

    // извлечь атрибуты ТУ из кода ТУ (заглушка)
    mod  = 1;                                           // модуль
    out  = id;                                          // выход (сейчас для имитации- порядковый номер ТУ)
    delay= 4000;                                        // задержка
}


// запись ТУ в основную (общую) очередь
// надо иметь версию функции без блокировки, чтобы при приеме нескольких ТУ положить их внутри одной блокировки
// это важно, если существуют спаренные ТУ, которые должны выбираться в одном цикле, иначе можно их разделить
void Tu::PushTu(WORD _tu, SUBSYSTYPE sys, ARM_TYPE arm, CANAL_TYPE ch)// ТУ - в очередь
{
    std::shared_ptr<Tu> tu(new Tu(_tu, sys, arm, ch), [](Tu *p) { Log (L"delete TU !!!"); delete p; } );
                                    //-------------------------------------------------  - так можно отследить факт удаления указателя,
                                    //                                                     можно убрать люмбда-функцию, включая запятую
    {
    std::lock_guard<std::mutex> locker(Tu::todo_lock);  // лучше освободить очередь до вызова SendMessage
    listToDo.push(tu);
    }
    SendMessage(new PaMessage(tuAckRcv, tu));
}

int Tu::todoSize()
{
    std::lock_guard <std::mutex> locker(todo_lock);
    return listToDo.size();
}

// получить символьное имя
std::wstring Tu::GetSysName()
{
    switch (sysType)                          // выбор по типу первой ТУ в очереди
    {
        case SYS_KP:    return L"КП"   ;    break;
        case SYS_RPC:   return L"РПЦ"  ;    break;
        case SYS_EBL:   return L"ЕБЛК" ;    break;
        case SYS_EM:    return L"ЭЦЕМ" ;    break;
        case SYS_MZF:   return L"МЗ-Ф" ;    break;
        case SYS_MPK:   return L"ЭЦМПК";    break;
        default:        return L" - "  ;    break;
    }
}

// --------------------------------------------------------------------------------------------------------------------------------------

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
        std::shared_ptr<Tu> tu = nullptr;                       // указатель на актуальную ТУ МТУ
        delayMpcRpc = 0;                                        // суммарная выдержка для ТУ МПЦ/РПЦ

        size_t n = 0;
		{
            std::lock_guard <std::mutex> locker(Tu::todo_lock); // блокировка todo_lock
            if ((n = Tu::listToDo.size()) > 0)
			{
                tu = Tu::listToDo.front();                      // выбираем ТУ, анализируем ее тип
                SUBSYSTYPE subsys = tu->GetSys();               // тип подсистемы очередной команды

                // если не КП - извлекаем и помещаем в нужную очередь все, подряд идущие, ТУ данного типа
                if (subsys != SYS_KP)
                {
                    std::queue <std::shared_ptr<Tu>> tmp;

                    while (tu->GetSys() == subsys)              // извлекаем и перемещаем все ТУ для данной подсистемы
                    {
                        tmp.push(tu);                           // помещаем во временую очередь
                        delayMpcRpc += 100;                     // подсчитываем задержку (обдумать алгоритм формирования значения)
                        Tu::listToDo.pop();                     // извлекаем из общей очереди
                        tu = Tu::listToDo.front();              // просматриваем следующую (без извлечения)
                    }
                    // выбрали все ТУ, поместили во временную очередь
                    MoveTu(tmp);                                // перемещаем ТУ в нужную очередь по типу подсистемы
                    continue;                                   // тип подсистемы верхней ТУ в очереди сменился, продолжаем цикл выборки
                }                                               // todo_lock разблокируется при этом

                // Обычная ТУ. Извлекаем для исполнения
                Tu::listToDo.pop();
			}
        }                                                       // разблокировка todo_lock

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


// перемещение ТУ из временной очереди в нужную по типу подсистемы
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


