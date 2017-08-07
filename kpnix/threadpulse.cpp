// поток опроса ТС

#include "main.h"

thread * pThreadPulse;								// указатель на поток формирования программного пульса

void ThreadPulse(long)
{
	
    threadsafecout(L"Поток программного пульса запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

    threadsafecout(L"Поток программного пульса завершен!");
	
}
