#include "QDateTime"
#include "logger.h"
#include "vector"
#include "defines.h"
#include "logger.h"
#include "sqlmessage.h"
#include "sqlserver.h"
#include "sqlblackbox.h"
#include "tcpheader.h"


// концепция, принятая в C#-реализации:
// класс "сообщение"    описывает одну запись, создается при вызове функции запись сообщения
// класс "сервер"       описывает одно подключение к серверу, содержит очередь сообщений и поток выборки и записи
// класс "SqlBlackbox"  описывает черный ящик с одним и более резервируемыми подключениями, содержит список серверов,
//                      и перегруженные функции записи сообщений, которые записывают сообщение в очереди всех серверов
// такая организация позволяет выполнять отложенную регистрацию сообщений без потери за счет накопления сообщений  вочереди
// на время недоступности сервера
//


SqlBlackBox::SqlBlackBox (QString mainstr, QString rsrvstr, Logger *logger)
{
    if (logger==nullptr)
        logger = new Logger("LOG/trstpsql.log", true, true);
    servers.push_back(new SqlServer (this, mainstr, logger));       // добавляем основной сервер
    if (rsrvstr != nullptr && rsrvstr.length())                     // если определен резервный
        servers.push_back(new SqlServer (this, rsrvstr, logger));   // добавляем резервный сервер
}

SqlBlackBox::~SqlBlackBox()
{
    while (servers.size())
    {
        SqlServer * p = servers.back();
        servers.pop_back();
        delete p;
    }
}

SqlServer* SqlBlackBox::GetSvr(int indx)
{
    return servers.size() >= indx ? servers[indx] : nullptr;
}

// запись сообщения
void SqlBlackBox::putMsg(int krug, int st, QString msg, int app, int event, QString ip)
{
    std::shared_ptr<SqlMessage> mes(new SqlMessage(krug, st, msg, app, event, ip));
    for (auto val : servers)
    {
        val->Add(mes);
    }
}

// запись сообщения (перегруженная)
void SqlBlackBox::putMsg(int st, QString msg, int app, int event)
{
    if (localhost.length()==0)
        localhost = GetHostIp();
    putMsg(0, st, msg, app, event, localhost);
}


