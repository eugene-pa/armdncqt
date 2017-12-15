#include "QDateTime"
#include "logger.h"
#include "vector"
#include "defines.h"
#include "logger.h"
#include "sqlmessage.h"
#include "sqlserver.h"
#include "sqlblackbox.h"


// концепция, принятая в C#-реализации:
// класс "сообщение"    описывает одну запись, создается при вызове функции запись сообщения
// класс "сервер"       описывает одно подключение к серверу, содержит очередь сообщений и поток выборки и записи
// класс "SqlBlackbox"  описывает черный ящик с одним и более резервируемыми подключениями, содержит список серверов,
//                      и перегруженные функции записи сообщений, которые записывают сообщение в очереди всех серверов
// такая организация позволяет выполнять отложенную регистрацию сообщений без потери за счет накопления сообщений  вочереди
// на время недоступности сервера
//



SqlBlackBox::SqlBlackBox (QString& mainstr, QString& rsrvstr, Logger *logger)
{
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



