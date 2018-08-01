#include <QSqlQuery>
#include <QSqlError>
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
bool SqlServer::logSql = false;                                 // флаг опционирования записи в лог текста запросов SQL
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
    name = QString("%1:%2").arg(params->host).arg(params->port);

    open();

    // создаем поток записи
    // pthread = new std::thread(ThreadDoSql, (long)this);
    pThreadPolling = std::unique_ptr<std::thread, ThreadTerminaterSql> (new std::thread(ThreadDoSql, (long)this));
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


// добавить сообщение
void SqlServer::Add (std::shared_ptr<SqlMessage> msg)
{
    std::lock_guard <std::mutex> locker(queue_lock); // блокировка todo_lock
    Messages.push(msg);
}

//
bool SqlServer::GetHosts(std::vector<QString>& hosts)
{
    QSqlDatabase db = open();
    if (db.isOpen())
    {
        QSqlQuery q("SELECT DISTINCT host FROM public.messages;", db);
        while (q.next())
        {
            hosts.push_back(q.value("host").toString());
        }
        return true;
    }
    return false;
}

// загрузить cправочники приложений и типов сообщений
void SqlServer::load()
{
    QSqlDatabase db = open();
    if (db.isOpen())
    {
        // загрузка имен приложений
        QSqlQuery q("SELECT * FROM public.appnames;", db);
        while (q.next())
            SqlBlackBox::appNames[q.value("app").toInt()] = q.value("appName").toString();

        // загрузка типов сообщений
        q = QSqlQuery("SELECT * FROM public.msgtypes;", db);
        while (q.next())
            SqlBlackBox::msgTypes[q.value("msgType").toInt()] = q.value("typename").toString();
    }
}

// время блокирования мьютекса exit_lock соответствует времени работы программы
// мьютекс блокируется в начале работы приложения, разблокируется при завершении
extern std::timed_mutex exit_lock;

// открытие БД
QSqlDatabase SqlServer::open()
{
    QSqlDatabase db = QSqlDatabase::database(name);
    if (!db.isOpen())
    {
        db = QSqlDatabase::addDatabase(params->driver, name);
        db.setHostName      (params->host);
        db.setDatabaseName  (params->bd);
        db.setUserName      (params->user);
        db.setPassword      (params->pwd);
        db.setPort          (params->port);
        db.open();
    }
    return db;
}

// поток выборки и записи сообщений
void SqlServer::ThreadDoSql(long param)
{
    if (param==0)
    {
        return;
    }
    SqlServer * server = (SqlServer*) param;
    server->counter = 0;

    server->Log("Запуск потока ThreadDoSql для сервера: " + server->connStr);
    while (!exit_lock.try_lock_for(std::chrono::milliseconds(1000)))
    {
        server->write();
/*
        std::queue <std::shared_ptr<SqlMessage>> tmp;           // временная очередь сообщений для записи в сервер
        {
            std::lock_guard <std::mutex> locker(server->queue_lock);//захватываем queue_lock
            // выбираем все сообщения во временную очередь (чтобы освободить основную очередь для записи)
            while (server->Messages.size())
            {
                std::shared_ptr<SqlMessage> p = server->Messages.front();
                tmp.push(p);
                server->Messages.pop();
            }
        }                                                       // освобождаем queue_lock

        // если выбрали сообщения (очередь не пуста) - поочередно записываем сообщения в сервер
        if (tmp.size())
        {
            // ВАЖНО: так как возможно подключение к нексольким серверам (например, основной и резервный),
            //        нельзя работать с соединением по умолчанию: надо именовать соединения, например blackbox1,blackbox2 и т.д.
            // 1. Запрашиваем соединение по умолчанию
            QSqlDatabase db = server->open();
            if (db.isOpen())
            {
                if (server->counter==-1)
                {
                    QString s = QString("Успешное подключение к серверу: '%1'").arg(server->connStr);
                    server->Log(s);
                    server->counter = 0;
                }
                // Записываем в БД все сообщения из временной очереди
                while (tmp.size())
                {
                    std::shared_ptr<SqlMessage> p = tmp.front();
                    tmp.pop();
                    QString sql = p->sql();

                    if (logSql)
                        server->Log(sql);                       // опционированный вывод в лог текста запроса

                    db.exec(sql);
                    if (db.lastError().isValid())
                    {
                        // Нюанс: так как таблица messages имеет внешние ключи, связанные с таблицами appnames, msgtypes, sources, 
                        // попытка вставить значения ключей, отсутствующие в этих таблицах, приводит к ошибкам
                        // Поэтому надо отслеживать использование неиспользуемых ключей и 
                        //  - либо добавлять их в таблицу
                        //  - либо использовать значение по умолчанию (0) для ключей, выходящих за допустимый диапазон
                        QString txt = QString("Ошибка '%1' при выполнения запроса: %2").arg(db.lastError().text()).arg(sql);
                        server->Log(txt);
                    }
                    else
                        server->counter++;
                }
            }
            else
            {
                // если ошибка первая после череды успешных записей - сообщение в лог
                if (server->counter!=-1)
                {
                    // проблемы открытия БД
                    QString s = QString("Проблемы подключения к серверу: '%1'").arg(server->connStr);
                    server->Log(s);
                }
                server->counter = -1;
            }
        }
*/
    }
    server->write();
    exit_lock.unlock();
    server->Log("Завершение потока ThreadSql для сервера: " + server->connStr);
}


// запись всех сообщений
void SqlServer::write()
{
    std::queue <std::shared_ptr<SqlMessage>> tmp;           // временная очередь сообщений для записи в сервер
    {
        std::lock_guard <std::mutex> locker(queue_lock);    //захватываем queue_lock
        // выбираем все сообщения во временную очередь (чтобы освободить основную очередь для записи)
        while (Messages.size())
        {
            std::shared_ptr<SqlMessage> p = Messages.front();
            tmp.push(p);
            Messages.pop();
        }
    }                                                       // освобождаем queue_lock

    // если выбрали сообщения (очередь не пуста) - поочередно записываем сообщения в сервер
    if (tmp.size())
    {
        // ВАЖНО: так как возможно подключение к нексольким серверам (например, основной и резервный),
        //        нельзя работать с соединением по умолчанию: надо именовать соединения, например blackbox1,blackbox2 и т.д.
        // 1. Запрашиваем соединение по умолчанию
        QSqlDatabase db = open();
        if (db.isOpen())
        {
            if (counter==-1)
            {
                QString s = QString("Успешное подключение к серверу: '%1'").arg(connStr);
                Log(s);
                counter = 0;
            }
            // Записываем в БД все сообщения из временной очереди
            while (tmp.size())
            {
                std::shared_ptr<SqlMessage> p = tmp.front();
                tmp.pop();
                QString sql = p->sql();

                if (logSql)
                    Log(sql);                                   // опционированный вывод в лог текста запроса

                db.exec(sql);
                if (db.lastError().isValid())
                {
                    // Нюанс: так как таблица messages имеет внешние ключи, связанные с таблицами appnames, msgtypes, sources,
                    // попытка вставить значения ключей, отсутствующие в этих таблицах, приводит к ошибкам
                    // Поэтому надо отслеживать использование неиспользуемых ключей и
                    //  - либо добавлять их в таблицу
                    //  - либо использовать значение по умолчанию (0) для ключей, выходящих за допустимый диапазон
                    QString txt = QString("Ошибка '%1' при выполнения запроса: %2").arg(db.lastError().text()).arg(sql);
                    Log(txt);
                }
                else
                    counter++;
            }
        }
        else
        {
            // если ошибка первая после череды успешных записей - сообщение в лог
            if (counter!=-1)
            {
                // проблемы открытия БД
                QString s = QString("Проблемы подключения к серверу: '%1'").arg(connStr);
                Log(s);
            }
            counter = -1;
        }
    }
}
