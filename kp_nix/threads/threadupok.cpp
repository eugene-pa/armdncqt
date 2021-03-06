﻿// поток работы с ОТУ УПОК+БРОК

#include <string>
#include <thread>
#include <mutex>
#include "common/common.h"
#include "common/pamessage.h"
#include "../kp2017.h"

void ThreadUpok(long)
{
    std::wstringstream s;
    s << L"Поток реализации ОТУ запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

	while (!exit_lock.try_lock_for(chronoMS(100)))
	{

	}
	exit_lock.unlock();

    s.str(std::wstring());
    s << L"Поток реализации ОТУ завершен. threadid=" << std::this_thread::get_id();
    Log(s.str());                        // отправка SendMessage здесь уже не проходит, так как запущен деструктор главного окна
}
