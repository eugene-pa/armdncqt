#ifndef SQLSERVER_H
#define SQLSERVER_H

#include "queue"
#include "memory"
#include "sqlmessage.h"
#include "sqlblackbox.h"
#include "logger.h"

// строка подключения:
// "DRIVER=QPSQL;Host=192.168.1.13;DATABASE=Blackbox;USER=postgres;PWD=358956;PORT=5434"
class SqlParams
{
    SqlParams(QString params);
    friend class SqlServer;
private:
    QString deiver;
    QString host;
    QString bd;
    QString user;
    QString pwd;
    int     port;
};

class SqlServer
{
public:
    SqlServer(class SqlBlackBox *, QString, Logger * );                 // конструктор подключения к серверу
    void Add (std::shared_ptr<SqlMessage> msg) { Messages.push(msg); }  // добавить сообщение
private:
    std::queue <std::shared_ptr<SqlMessage>> Messages;                  // очередь сообщений для записи в сервер
    QString connStr;                                                    // строка подключения
    class SqlBlackBox * parent;                                               // родительсий класс
    Logger * logger;
    ulong counter;                                                      // счетчик обоаюотанных сообщений
};

#endif // SQLSERVER_H
