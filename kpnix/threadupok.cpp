﻿// поток отработки ОТУ УПОК+БРОК

#include "main.h"

thread * pThreadUpok;											// указатель на поток обработки ОТУ УПОК+БРОК
void ThreadUpok(long)
{
	threadsafecout("Поток UPOK запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

	threadsafecout("Поток UPOK завершен!");
}