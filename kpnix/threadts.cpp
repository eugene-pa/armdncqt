// поток опроса ТС

#include "main.h"

BYTE TsStatus		[TSSIZE];								// битовый массив мгновенного состояния ТС
BYTE PulseStatus	[TSSIZE];								// битовый массив вычисленного состояния мигания ТС
BYTE TsStatusPrv	[TSSIZE];								// битовый массив мгновенного состояния ТС в предыдущем цикле опроса
BYTE PulseStatusPrv [TSSIZE];								// битовый массив вычисленного состояния мигания ТС в предыдущем цикле опроса

BYTE TsStatus2		[TSSIZE];								// битовый массив мгновенного состояния ТС смежного блока
BYTE PulseStatus2	[TSSIZE];								// битовый массив вычисленного состояния мигания ТС смежного блока

BYTE CompareErrors  [TSSIZE];								// битовый массив ошибок сравнения (1-ошибка сравнения, 0 - ОК)

thread * pThreadTs;											// указатель на поток опроса ТС

void ThreadTS(long)
{
	threadsafecout("Поток TS запущен!");

	// Циклический опрос ТС вплоть до запроса выхода (освобождение exit_lock)
	while (!exit_lock.try_lock_for(chronoMS(1000)))
	{
		threadsafecout("Опрос ТС");
	}
	exit_lock.unlock();

	threadsafecout("Поток TS завершен!");
}
