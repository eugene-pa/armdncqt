#ifndef THREADTU_H
#define THREADTU_H

#include <mutex>
#include <queue>
#include "common/common.h"

// Класс-оболочка для команды ТУ
// Имеет смыслрасширить класс таким образом, чтобы в конструкторе
// извлечь из DWORD все, что нужно: система, задержка, коррдинаты и т.д.
// Тогда не надо будет делать это в разных местах кода: например при отображении ТУ, при выдаче ТУ и т.д.
class Tu
{

public:
    Tu(DWORD tu);
    static DWORD Counter;                                   // статический счетчик ТУ для идентификации
    static std::queue <std::shared_ptr<Tu>> listToDo;       // очередь ТУ на исполнение
    static std::mutex todo_lock;                            // блокировка доступа к очереди TODO
    static int todoSize();                                  // размер очереди на выполнение
    static void PushTu(DWORD tu);

    DWORD GetTu   () { return tu;   }
    DWORD GetId   () { return id;   }
    int   GetMod  () { return mod;  }
    int   GetOut  () { return out;  }
    int   GetDelay() { return delay;}
    time_t GetTrcv() { return tRcv; }
    time_t GetTbeg() { return tBeg; }
    time_t GetTend() { return tEnd; }

private:
    DWORD id;                                               // порядковый номер ТУ
    DWORD tu;                                               // код ТУ
    // type                                                 // тип МТУ/РПЦ/МПЦ и  т.д.
    int   mod;                                              // модуль
    int   out;                                              // выход
    int   delay;                                            // задержка
    time_t tRcv;                                            // время приема
    time_t tBeg;                                            // время начала выдачи
    time_t tEnd;                                            // время окончания выдачи (если не использовать очередь исполненных - лишний член)
};


#endif // THREADTU_H
