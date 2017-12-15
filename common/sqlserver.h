#ifndef SQLSERVER_H
#define SQLSERVER_H

#include "memory"
#include "mutex"
#include "thread"

// строка подключения:
// "DRIVER=QPSQL;Host=192.168.1.13;DATABASE=Blackbox;USER=postgres;PWD=358956;PORT=5434"
class SqlParams
{
public:
    SqlParams(QString params);
    friend class SqlServer;
//private:
    QString GetOption(const char * option, QString src);
    QString driver;
    QString host;
    QString bd;
    QString user;
    QString pwd;
    int     port;
};


// класс-deleter для завершения рабочих потоков; используется в смарт-указателях std::unique_ptr
// выполняет: ожидание завершения, вывод в лог и удаление указателя
class ThreadTerminater
{
public:
    void operator () (std::thread * p)
    {
        auto id = p->get_id();                                          // запоминаем id, пока поток живой
        p->join();                                                      // ожидаем завершения потока
        //std::wstringstream s;
        //s << L"Удаление указателя на поток " << id;                     // если хотим убедиться в удалении указателя
        //Log(s.str());                                                   // выводим лог
        delete p;                                                       // удаляем указатель
    }
};


class SqlServer
{
public:
    SqlServer(class SqlBlackBox *, QString&, Logger * );                 // конструктор подключения к серверу
    ~SqlServer();
    void Add (std::shared_ptr<SqlMessage> msg) { Messages.push(msg); }  // добавить сообщение

    std::mutex queue_lock;
    std::queue <std::shared_ptr<SqlMessage>> Messages;                  // очередь сообщений для записи в сервер
    SqlParams * GetParams() { return params; }
private:

    QString connStr;                                                    // строка подключения
    class SqlBlackBox * parent;                                         // родительсий класс
    Logger * logger;
    ulong counter;                                                      // счетчик обоаюотанных сообщений
    SqlParams * params;
    std::thread * pthread;
    std::unique_ptr<std::thread, ThreadTerminater> pThreadPolling;
};



#endif // SQLSERVER_H
