#include <thread>
#include <mutex>
#include "sqlserver.h"

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

}


// конструктор подключения к серверу
// строка подключения:
// "DRIVER=QPSQL;Host=192.168.1.13;DATABASE=Blackbox;USER=postgres;PWD=358956;PORT=5434"
SqlServer::SqlServer(SqlBlackBox * parent, QString connstr, Logger * logger)
{
    counter = 0;
    this->logger = logger;
    this->parent = parent;
    this->connStr= connstr;
}

// время блокирования мьютекса exit_lock соответствует времени работы программы
// мьютекс блокируется в начале работы приложения, разблокируется при завершении
extern std::timed_mutex exit_lock;
void ThreadDoSql(long param)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("basicaccount");
    db.setUserName("postgres");
    db.setPassword("root");
    db.setPort(5435);

    //Log("Запуск потока ThreadSql");
    while (!exit_lock.try_lock_for(std::chrono::milliseconds(100)))
    {

    }
    exit_lock.unlock();
    //Log("Завершение потока ThreadSql");
}
