// поток опроса ТС

#include <string>
#include <thread>
#include <mutex>
#include "common/common.h"
#include "common/pamessage.h"
#include "../kp2017.h"

BYTE TsStatus		[TSSIZE];								// битовый массив мгновенного состояния ТС
BYTE PulseStatus	[TSSIZE];								// битовый массив вычисленного состояния мигания ТС
BYTE TsStatusPrv	[TSSIZE];								// битовый массив мгновенного состояния ТС в предыдущем цикле опроса
BYTE PulseStatusPrv [TSSIZE];								// битовый массив вычисленного состояния мигания ТС в предыдущем цикле опроса

BYTE TsStatus2		[TSSIZE];								// битовый массив мгновенного состояния ТС смежного блока
BYTE PulseStatus2	[TSSIZE];								// битовый массив вычисленного состояния мигания ТС смежного блока

BYTE CompareErrors  [TSSIZE];								// битовый массив ошибок сравнения (1-ошибка сравнения, 0 - ОК)

void ThreadTS(long)
{
    std::wstringstream s;
    s << L"Поток опроса ТС запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

	// Циклический опрос ТС вплоть до запроса выхода (освобождение exit_lock)
	while (!exit_lock.try_lock_for(chronoMS(1000)))
	{
        //SendMessage(new PaMessage(L"Опрос ТС"));
	}
	exit_lock.unlock();

    s.str(std::wstring());
    s << L"Поток опроса ТС завершен. threadid=" << std::this_thread::get_id();
    Log(s.str());                        // отправка SendMessage здесь уже не проходит, так как запущен деструктор главного окна
}
