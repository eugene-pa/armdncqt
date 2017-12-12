#include <thread>
#include <mutex>
#include "sqlblackbox.h"

// концепция, принятая в C#-реализации:
// класс "сообщение"    описывает одну запись, создается при вызове функции запись сообщения
// класс "сервер"       описывает одно подключение к серверу, содержит очередь сообщений и поток выборки и записи
// класс "SqlBlackbox"  описывает черный ящик с одним и более резервируемыми подключениями, содержит список серверов,
//                      и перегруженные функции записи сообщений, которые записывают сообщение в очереди всех серверов
// такая организация позволяет выполнять отложенную регистрацию сообщений без потери за счет накопления сообщений  вочереди
// на время недоступности сервера
//

std::queue<SqlBlackBox*> SqlBlackBox::Messages;
extern std::timed_mutex exit_lock;

SqlBlackBox::SqlBlackBox(int krug, int st, QString& ip, int app, int event, QDateTime tmdt, QString& s)
{
    idKrug  = krug;
    idSt    = st;
    host    = ip;
    idApp   = app;
    idEvent = event;
    t       = tmdt;
    msg     = s;
    Messages.push(this);
}


void ThreadDoSql(long)
{
    //Log("Запуск потока ThreadSql");
    while (!exit_lock.try_lock_for(std::chrono::milliseconds(100)))
    {

    }
    exit_lock.unlock();
    //Log("Завершение потока ThreadSql");
}

