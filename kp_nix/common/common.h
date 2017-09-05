#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <memory>                                           // for unique_ptr

#define DBG_INCLUDE

extern std::timed_mutex exit_lock;							// мьютекс, разрешающий завершение приложения

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

#define LOBYTE(w)     ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE(w)     ((BYTE)((((DWORD)(w)) & 0xff) >> 8))
#define MAKEWORD(a,b) ((WORD)(((BYTE)(a))|(((WORD)((BYTE)(b)))<<8)))

extern WORD GetCRC (BYTE *buf,WORD Len);                    // побайтовый алгоритм вычисления CRC
extern std::wstring GetHexW (void *data, int length);       // получить HEX-представление массива байт
extern std::timed_mutex exit_lock;                          // блокировка до выхода
extern void Log(std::wstring);

// Макрос милисекундного Sleep
#define chronoMS(n) std::chrono::milliseconds(n)
#define SleepMS(n) std::this_thread::sleep_for(chronoMS(n))

void Log (std::wstring s);                                  // лог
void SendMessage (class PaMessage *);                       // отправка сообщения главному окну (см.mainwindow.cpp)


#endif // COMMON_H
