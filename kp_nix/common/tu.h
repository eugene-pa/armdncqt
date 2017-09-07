#ifndef TU_H
#define TU_H

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
    Tu(WORD tu, SUBSYSTYPE sys=SYS_KP, ARM_TYPE arm = ARM_DNC, CANAL_TYPE ch = CANAL_1);
    static DWORD Counter;                                   // статический счетчик ТУ для идентификации
    static std::queue <std::shared_ptr<Tu>> listToDo;       // очередь ТУ на исполнение
    static std::mutex todo_lock;                            // блокировка доступа к очереди TODO
    static int todoSize();                                  // размер очереди на выполнение
    static void PushTu(WORD tu, SUBSYSTYPE sys=SYS_KP, ARM_TYPE arm = ARM_DNC, CANAL_TYPE ch = CANAL_1);

    WORD GetTu       () { return tu;      }
    DWORD GetId      () { return id;      }
    int   GetMod     () { return mod;     }
    int   GetOut     () { return out;     }
    int   GetDelay   () { return delay;   }
    time_t GetTrcv   () { return tRcv;    }
    time_t GetTbeg   () { return tBeg;    }
    time_t GetTend   () { return tEnd;    }
    SUBSYSTYPE GetSys() { return sysType; }
    std::wstring GetSysName(){ return ::GetSysName(sysType); }    // получить имя подсистемы

private:
    DWORD id;                                               // порядковый номер ТУ
    WORD tu;                                                // код ТУ
    // type                                                 // тип МТУ/РПЦ/МПЦ и  т.д.
    int   mod;                                              // модуль
    int   out;                                              // выход
    int   delay;                                            // задержка
    time_t tRcv;                                            // время приема
    time_t tBeg;                                            // время начала выдачи
    time_t tEnd;                                            // время окончания выдачи (если не использовать очередь исполненных - лишний член)

    SUBSYSTYPE sysType;                                     // тип ЭЦ, на который направляется ТУ
    ARM_TYPE   armType;                                     // тип АРМ, выдавший ТУ
    CANAL_TYPE canalType;                                   // тип канала (используются для идентификации канала ТУ ЭЦ-МПК)
};
#endif // TU_H

