// поток опроса линии связи

#include "main.h"
#include "../common/rsasinc.h"

thread * pThreadPolling;							// указатель на поток опроса линии связи

void ThreadPolling(long)
{
    threadsafecout(L"Поток опроса линни связи запущен!");
    RsAsinc rs ("COM3,38400,N,8,1");

    while (!exit_lock.try_lock_for(chronoMS(100)))
    {
        BYTE data[4048];
        try
        {
            int indx = 0;
            int ch;
            // ждем маркер
            if ((ch = rs.GetCh()) != 1)     continue;
            data[indx++] = (BYTE) ch;
            data[indx++] = (BYTE) rs.GetChEx();
            data[indx++] = (BYTE) rs.GetChEx();
            int l = data[1] * 256 + data[2];

            for (int i=0; i<l; i++)
            {
                data[indx++] = rs.GetChEx();
            }
            int crclo = rs.GetChEx();
            int crchi = rs.GetChEx();
        }
        catch (...)
        {
            continue;
        }
    }
    exit_lock.unlock();

    threadsafecout(L"Поток опроса линни связи завершен!");

}

