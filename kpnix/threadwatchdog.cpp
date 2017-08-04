// поток опроса ТС

#include "main.h"

thread * pThreadWatchDog;							// указатель на поток включения и управления сторожевым таймером

void ThreadWatchDog(long)
{
	threadsafecout("Поток управления сторожевым таймером запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

	threadsafecout("Поток управления сторожевым таймером завершен!");
	
}
