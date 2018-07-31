#ifndef SQLSERVER_H
#define SQLSERVER_H

#include <QSqlDatabase>
#include <QDebug>
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
class ThreadTerminaterSql
{
public:
    void operator () (std::thread * p)
    {
        //auto id = p->get_id();                                          // запоминаем id, пока поток живой
        p->join();                                                      // ожидаем завершения потока
        qDebug() << "Удаление указателя на поток ";                     // если хотим убедиться в удалении указателя
        delete p;                                                       // удаляем указатель
    }
};


class SqlServer
{
public:
    static bool logSql;                                                 // флаг опционирования записи в лог текста запросов SQL

    SqlServer(class SqlBlackBox *, QString&, Logger * );                // конструктор подключения к серверу
    ~SqlServer();
    void Add (std::shared_ptr<SqlMessage> msg);                         // добавить сообщение

    std::mutex queue_lock;
    std::queue <std::shared_ptr<SqlMessage>> Messages;                  // очередь сообщений для записи в сервер
    void Log(QString s) { if (logger) logger->log(s); }
    static void ThreadDoSql(long param);
    QString Name() { return name; }
    bool  GetHosts(std::vector<QString>& hosts);
    void load();                                                        // загрузить cправочники приложений и типов сообщений
private:
    int    counter;                                                     // счетчик подряд идущих успешных записей (без ошибок)
    QString connStr;                                                    // строка подключения
    class SqlBlackBox * parent;                                         // родительсий класс
    Logger * logger;                                                    // логгер
    SqlParams * params;                                                 // параметры соединения
    QString name;                                                       // имя соединения: IP:порт
    std::unique_ptr<std::thread, ThreadTerminaterSql> pThreadPolling;     // указатель на рабочий поток записи сообщений

    QSqlDatabase open();                                                // открытие БД
};



#endif // SQLSERVER_H
