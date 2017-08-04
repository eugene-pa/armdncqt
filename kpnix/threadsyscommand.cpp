// поток опроса ТС

#include "main.h"

thread * pThreadSysCommand;							// указатель на поток исполнения директив управления КП

void ThreadSysCommand(long)
{
	threadsafecout("Поток реализации директив управления КП запущен!");

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

	threadsafecout("Поток реализации директив управления КП завершен!");
	
}
