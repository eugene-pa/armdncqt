#ifndef MAIN_H
#define MAIN_H

#include <QIODevice>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include <iostream>											// std::cout
#include <thread>											// std::thread
#include <mutex>											// мьютексы
#include <queue>											// FIFO
#include <cstdio>

#ifdef Q_OS_WIN
#include <io.h>
#include <fcntl.h>
#endif // #ifdef Q_OS_WIN

#include "kp2017.h"

using namespace std;

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

std::wstring qToStdWString(const QString &str);             // QString --> std::wstring
QString stdWToQString(const std::wstring &str);             // std::wstring --> QString

void threadsafecout(const wchar_t *);                       // безопасный (с блокировкой мьютекса) вывод строки символов на консоль с указанием потока
void threadsafecout(wstring msg);                           // безопасный (с блокировкой мьютекса) вывод строки на консоль

// Базовые потои КП
void   ThreadTS				(long);							// функция потока опроса ТС
void   ThreadTU				(long);							// функция потока вывода ТУ
void   ThreadUpok			(long);							// функция потока обработки ОТУ УПОК+БРОК
void   ThreadSysCommand     (long);							// функция потока исполнения директив управления КП
void   ThreadPulse			(long);							// функция потока формирования программного пульса
void   ThreadMonitoring		(long);							// функция потока мониторинга состояния КП
void   ThreadTestTU			(long);							// функция потока циклического теста ТУ
void   ThreadWatchDog		(long);							// функция потока включения и управления сторожевым таймером

extern thread * pThreadTs;									// указатель на поток опроса ТС
extern thread * pThreadTu;									// указатель на поток вывода ТУ
extern thread * pThreadUpok;								// указатель на поток обработки ОТУ УПОК+БРОК
extern thread * pThreadSysCommand;							// указатель на поток исполнения директив управления КП
extern thread * pThreadPulse;								// указатель на поток формирования программного пульса
extern thread * pThreadMonitoring;							// указатель на поток мониторинга состояния КП
extern thread * pThreadTestTU;								// указатель на поток циклического теста ТУ
extern thread * pThreadWatchDog;							// указатель на поток включения и управления сторожевым таймером

extern  std::mutex con_lock;								// доступ к консоли
extern  std::timed_mutex exit_lock;							// блокировка до выхода
extern  bool  rqExit;										// запрос выхода

void PushTu(DWORD tu);										// ТУ в очередь на исполнение

// Макрос милисекундного Sleep
#define chronoMS(n) std::chrono::milliseconds(n)
#define SleepMS(n) std::this_thread::sleep_for(chronoMS(n))

#endif // MAIN_H
