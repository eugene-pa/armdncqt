﻿// поток опроса ТС

#include "main.h"

thread * pThreadTestTU;								// указатель на поток циклического теста ТУ

void ThreadTestTU(long)
{
    threadsafecout(L"Поток тестирования модулей ТУ запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

    threadsafecout(L"Поток тестирования модулей ТУ завершен!");
	
}
