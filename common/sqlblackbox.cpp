#include "QDateTime"
#include <QDebug>
#include "logger.h"
#include "vector"
#include "defines.h"
#include "logger.h"
#include "sqlmessage.h"
#include "sqlserver.h"
#include "sqlblackbox.h"
#include "tcpheader.h"


// концепция Blackbox

// класс черный ящик    SqlBlackbox -  описывает черный ящик с одним и более резервируемыми серверами, содержит список серверов,
//                                    и перегруженные функции записи сообщений, которые записывают сообщение
//                                    в очереди всех серверов
// класс "сервер"       SqlServer   - описывает одно подключение к серверу, содержит очередь сообщений и поток выборки и записи
// класс "сообщение"    SqlMessage  - описывает одну запись, создается при вызове функции запись сообщения SqlBlackBox::putMsg
//                                    при создании сообщения создается разделяемый указатель
// класс "параметры"    SqlParams   - содержит параметры подключения, определяемые при разборе строки подключения
//
// такая организация позволяет выполнять отложенную регистрацию сообщений без потери за счет накопления сообщений  вочереди
// на время недоступности сервера
//

std::unordered_map <int, QString  > SqlBlackBox::appNames;          // имена приложений, индексированные по ID
std::unordered_map <int, QString  > SqlBlackBox::msgTypes;          // имена типов сообщений, индексированные по ID


// конструктор принимает до 2-х подключений; можно масштабировать более чем на 2
SqlBlackBox::SqlBlackBox (QString& mainstr, QString& rsrvstr, Logger *logger)
{
    if (logger==nullptr)                                            // если не передали логгер, создам свой
        logger = new Logger("LOG/SqlBlackBox.log", true, true);
    this->logger = logger;
    servers.push_back(new SqlServer (this, mainstr, logger));       // добавляем основной сервер
    if (rsrvstr != nullptr && rsrvstr.length())                     // если определен резервный
        servers.push_back(new SqlServer (this, rsrvstr, logger));   // добавляем резервный сервер
    load();                                                         // загрузить мправочники приложений и типов сообщений
}

// деструктор удаляет созданные экземпляры SqlServer
SqlBlackBox::~SqlBlackBox()
{
    while (servers.size())
    {
        SqlServer * p = servers.back();
        servers.pop_back();
        delete p;
    }
}



// загрузить cправочники приложений и типов сообщений
void SqlBlackBox::load()
{
    for (auto val : servers)
    {
        val->load();
    }
}


// получить ключ приложения по имени или 0
int SqlBlackBox::GetAppIdByName (QString s)
{
    for (auto val : appNames)
    {
        if (s == val.second)
            return val.first;
    }
    return 0;
}


// получить ключ типа сообщения по имени или 0
int SqlBlackBox::GetMsgTypeByName(QString s)
{
    for (auto val : msgTypes)
    {
        if (s == val.second)
            return val.first;
    }
    return 0;
}



// получить сервер по индексу
SqlServer* SqlBlackBox::GetSvr(int indx)
{
    return (int)servers.size() >= indx ? servers[indx] : nullptr;
}

// запись сообщения
void SqlBlackBox::putMsg(int krug, int st, QString msg, int app, int event, QString ip)
{
    // если IP не задан, определяем локальный
    if (ip.length()==0)
    {
        if (localhost.length()==0)
            localhost = GetHostIp();
        ip = localhost;
    }
    std::shared_ptr<SqlMessage> mes(new SqlMessage(krug, st, msg, app, event, ip), [](SqlMessage *p) { qDebug() << "delete SqlMessage"; delete p; } );
                                                                                   // так можно отследить факт удаления указателя,
                                                                                   // можно убрать люмбда-функцию, включая запятую
    // добавляем указатель в очереди всех серверов
    for (auto val : servers)
    {
        val->Add(mes);
    }

    //logger->LogStr(QString("Ст.%1. %2").arg(st).arg(msg));
}


// запись сообщения с текстовыми параметрами
void SqlBlackBox::putMsg(QString msg, QString app, QString event, QString ip, int krug, int st)
{
    putMsg(krug, st, msg, (int)(GetAppIdByName (app)), (int)(GetMsgTypeByName(event)), ip);
}


// запись сообщения (перегруженная без круга и IP)
void SqlBlackBox::putMsg(int st, QString msg, int app, int event)
{
    // определяемся с локальным IP, круг по умолчанию = 0
    if (localhost.length()==0)
        localhost = GetHostIp();
    putMsg(0, st, msg, app, event, localhost);
}



