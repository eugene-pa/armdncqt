#ifndef THREADTU_H
#define THREADTU_H

#include <mutex>
#include <queue>
#include "common/common.h"

extern std::mutex todo_lock;                                // блокировка доступа к очереди TODO/DONE
extern std::mutex done_lock;								// блокировка доступа к очереди DONE
extern std::queue<DWORD> listToDo;							// очередь ТУ на исполнение
extern std::queue<DWORD> listDone;							// очередь исполненных ТУ

extern int todoSize();                                      // размер очереди на выполнение
extern int doneSize();                                      // размер очереди выполненных ТУ

#ifdef DBG_INCLUDE
extern void DBG_PushTu(DWORD tu);                           // имитация отправки ТУ
#endif // #ifdef DBG_INCLUDE

#endif // THREADTU_H
