// поток опроса ТС

#include "main.h"

thread * pThreadMonitoring;							// указатель на поток мониторинга состояния КП

void ThreadMonitoring(long)
{
    threadsafecout(L"Поток мониторинга запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

    threadsafecout(L"Поток мониторинга завершен!");
	
}
