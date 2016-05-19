#include "servertcp.h"
#include "clienttcp.h"

ServerTcp::ServerTcp(quint16 port, QHostAddress bind, Logger * logger)       // конструктор получает порт и, возможно, интерфейс привязки
{
    this->port = port;
    this->bind = bind;
    this->logger = logger;

    tcpServer = new QTcpServer();
    QObject::connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    tcpServer->listen(bind, port);

    log(msg = QString("Конструктор ServerTcp %1:%2").arg(bind.toString()).arg(port));
}

ServerTcp::~ServerTcp()
{

}

// уведомление об ошибке сервера
void ServerTcp::acceptError(QAbstractSocket::SocketError socketError)
{

}

// уведомление о подключении нового клиента
void ServerTcp::newConnection()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    ClientTcp * client = new ClientTcp(this, clientConnection, logger);
    clients.append(client);                                 // добавить подключенного клиента в список
    QObject::connect(client, SIGNAL(dataready (ClientTcp*)), this, SLOT(dataready (ClientTcp*)));
    QObject::connect(client, SIGNAL(rawdataready (ClientTcp*)), this, SLOT(rawdataready (ClientTcp*)));
}

void ServerTcp::log (QString& msg)
{
    if (logger != nullptr)
        logger->log(msg);
    else
        qDebug() << msg;
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void ServerTcp::dataready (ClientTcp * client)
{
    client->SendAck();                                      // квитирование
    qDebug() << Logger::GetHex(client->Data(), client->Length());
}

// приняты неформатные данные - идентификация клиента
// сервер должен решить, разрешено ли обслуживание клиента
void ServerTcp::rawdataready (ClientTcp * client)
{
    //client->SendAck();                                      // квитирование
    // обработка данных
    qDebug() << Logger::GetHex(client->RawData(), client->RawLength());
}

void ServerTcp::sendToAll(char * data, quint16 length)
{
    foreach (ClientTcp * client, clients)
    {
        try
        {
            client->Send(data, length);
        }
        catch (...)
        {

        }
    }
}
