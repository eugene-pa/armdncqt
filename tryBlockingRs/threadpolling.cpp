#include <string>
#include "../common/blockingrs.h"

void Log(std::wstring);

void ThreadPolling(long param)
{
    QString config = QString::fromStdWString(*(std::wstring *)param);
    BlockingRS rs(config);
    rs.start();

//    QByteArray bt("09876");
//    rs.Send(bt);

    while (true)
    {
        int ch;
        if ((ch = rs.GetCh()) == -1)               // используем функцию без исключений пока ждем маркер
            continue;
        //QByteArray bt ((char *)&ch,1);
        //rs.Send(bt);
        std::wstring s = std::to_wstring(ch);
        Log(s);
    }
}
