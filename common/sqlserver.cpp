#include "QDateTime"
#include <thread>
#include <mutex>
#include "queue"
#include "memory"
#include "logger.h"
#include "sqlmessage.h"
#include "sqlserver.h"
#include "sqlblackbox.h"

// регулярные выражения для извлечения пааметров строки подключения вида
// "DRIVER=QPSQL;Host=192.168.1.13;PORT=5434;DATABASE=Blackbox;USER=postgres;PWD=358956"
const char * parseDriver   = "(?<=[Dd][Rr][Ii][Vv][Ee][Rr]=)[^;^z]+";           // DRIVER
const char * parseHost     = "(?<=[Hh][Oo][Ss][Tt]=)[^;^z]+";                   // Host
const char * parsePort     = "(?<=[Pp][Oo][Rr][Tt]=)[^;^z]+";                   // Port
const char * parseDatabase = "(?<=[Dd][Aa][Tt][Aa][Bb][Aa][Ss][Ee]=)[^;^z]+";   // DATABASE
const char * parseUser     = "(?<=[Uu][Ss][Ee][Rr]=)[^;^z]+";                   // USER
const char * parsePassword = "(?<=[Pp][Ww][Dd]=)[^;^z]+";                       // PWD

SqlParams::SqlParams(QString params)
{
    driver = GetOption(parseDriver  , params);
    host   = GetOption(parseHost    , params);
    bd     = GetOption(parseDatabase, params);
    user   = GetOption(parseUser    , params);
    pwd    = GetOption(parsePassword, params);
    port   = GetOption(parsePort    , params).toInt();
}

QString SqlParams::GetOption(const char * option, QString src)
{
    QRegularExpression regexEx(option);
    QRegularExpressionMatch match = regexEx.match(src);
    return match.hasMatch() ? match.captured() : "";
}

void ThreadDoSql(long param);
// =========================================================================================
// конструктор подключения к серверу
// строка подключения:
// "DRIVER=QPSQL;Host=192.168.1.13;DATABASE=Blackbox;USER=postgres;PWD=358956;PORT=5434"
SqlServer::SqlServer(SqlBlackBox * parent, QString& connstr, Logger * logger)
{
    counter = 0;
    this->logger = logger;
    this->parent = parent;
    this->connStr= connstr;

    // инициализация параметров
    params = new SqlParams(connstr);

    // создаем поток записи
    // pthread = new std::thread(ThreadDoSql, (long)this);
    pThreadPolling = std::unique_ptr<std::thread, ThreadTerminater> (new std::thread(ThreadDoSql, (long)this));
}

SqlServer::~SqlServer()
{
    if (params)
        delete params;
//    if (pthread)
//    {
//        pthread->join();
//        delete pthread;
//    }
}

// время блокирования мьютекса exit_lock соответствует времени работы программы
// мьютекс блокируется в начале работы приложения, разблокируется при завершении
extern std::timed_mutex exit_lock;
void ThreadDoSql(long param)
{
    SqlServer * server = (SqlServer*) param;

    //Log("Запуск потока ThreadSql");
    while (!exit_lock.try_lock_for(std::chrono::milliseconds(1000)))
    {
        {
        std::queue <std::shared_ptr<SqlMessage>> tmp;            // временная очередь сообщений для записи в сервер

        // захватываем queue_lock
        std::lock_guard <std::mutex> locker(server->queue_lock); // блокировка todo_lock
        while (server->Messages.size())
        {
            std::shared_ptr<SqlMessage> p = server->Messages.front();
            tmp.push(p);
            server->Messages.pop();
        }
        // освобождаем queue_lock

        // если выбрали сообщения - пишем их в сервер
        //if (tmp.size())
        {
            // 1. Подключаемся к серверу
            QSqlDatabase db = QSqlDatabase::addDatabase(server->GetParams()->driver);
            db.setHostName      (server->GetParams()->host);
            db.setDatabaseName  (server->GetParams()->bd);
            db.setUserName      (server->GetParams()->user);
            db.setPassword      (server->GetParams()->pwd);
            db.setPort          (server->GetParams()->port);
            if (db.open())
            {
                // 2. Если подключены - пишем
                while (tmp.size())
                {
                    std::shared_ptr<SqlMessage> p = tmp.front();
                    tmp.pop();
                    // пишем p
                }
            }
            else
            {
                // проблемы открытия БД
                qDebug() << "Error Open BD";
            }
        }

        }
    }
    exit_lock.unlock();
    //Log("Завершение потока ThreadSql");
}

