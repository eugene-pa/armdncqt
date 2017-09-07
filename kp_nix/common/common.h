#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <memory>                                           // for unique_ptr

#define DBG_INCLUDE

// типы микропроцессорных МПЦ/РПЦ (используются для идентификации ТУ)
enum SUBSYSTYPE
{
    SYS_KP,
    SYS_RPC,
    SYS_EBL,
    SYS_EM,
    SYS_MZF,
    SYS_MPK,
    SYS_MAX,
};

// типы АРМ, подключаемых к КП (используются для идентификации ТУ)
enum ARM_TYPE
{
    ARM_DNC,
    ARM_DSP,
    ARM_DBG,
    ARM_MAX,
};

// тип канала (используются для идентификации канала ТУ ЭЦ-МПК)
enum CANAL_TYPE
{
    CANAL_1,
    CANAL_2,
};

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
std::wstring GetSysName(SUBSYSTYPE);                        // имя системы ЭЦ по типу



#endif // COMMON_H
