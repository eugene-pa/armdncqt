#include "servertcp.h"
#include "clienttcp.h"

ServerTcp::ServerTcp(quint16 port, QHostAddress bind, Logger * logger)       // конструктор получает порт и, возможно, интерфейс привязки
{
    this->port = port;
    this->bind = bind;
    this->logger = logger;

    tcpServer = new QTcpServer();
    QObject::connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

//  tcpServer->listen(bind, port);

    log(msg = QString("Конструктор ServerTcp %1:%2").arg(bind.toString()).arg(port));
}

ServerTcp::~ServerTcp()
{

}

void ServerTcp::start()
{
    tcpServer->listen(bind, port);
    log(msg = QString("Старт сервера ServerTcp %1:%2").arg(bind.toString()).arg(port));
}

// уведомление об ошибке сервера
void ServerTcp::slotAcceptError(ClientTcp * conn)
{
    emit acceptError (conn);                                // ошибка на сокете
}

// уведомление о подключении нового клиента
void ServerTcp::slotNewConnection()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    ClientTcp * client = new ClientTcp(this, clientConnection, logger);
    _clients.append(client);                                 // добавить подключенного клиента в список
    QObject::connect(client, SIGNAL(dataready    (ClientTcp*)), this, SLOT(slotDataready    (ClientTcp*)));
    QObject::connect(client, SIGNAL(rawdataready (ClientTcp*)), this, SLOT(slotRawdataready (ClientTcp*)));
    QObject::connect(client, SIGNAL(disconnected (ClientTcp*)), this, SLOT(slotDisconnected (ClientTcp*)));
    QObject::connect(client, SIGNAL(error        (ClientTcp*)), this, SLOT(slotAcceptError  (ClientTcp*)));
    QObject::connect(client, SIGNAL(roger        (ClientTcp*)), this, SLOT(slotRoger        (ClientTcp*)));

    QString s = QString("ServerTcp [порт %1]. Подключен клиент %2").arg(port).arg(client->name());
    log(s);

    emit newConnection(client);
}

void ServerTcp::log (QString& msg)
{
    if (logger != nullptr)
        logger->log(msg);
    else
        qDebug() << msg;
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void ServerTcp::slotDataready (ClientTcp * client)
{
//  client->SendAck();                                      // квитирование
    //qDebug() << Logger::GetHex(client->data(), client->length());

    emit dataready(client);

    client->clear();
}

// приняты неформатные данные - идентификация клиента
// сервер должен решить, разрешено ли обслуживание клиента
void ServerTcp::slotRawdataready (ClientTcp * client)
{
    // прием символьногого идентификатора типа клиента (Шлюз СПД, ГИД УРАЛ и т.д.)
    // сейчас используется кодировка "Windows-1251", поэтому используем декодер
    QByteArray msg(client->rawData());
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    client->setid(codec->toUnicode(msg));
    client->clear();
    qDebug() << QString("Идентификация клиента: %1").arg(client->getid());
}

// разрыв соединения
void ServerTcp::slotDisconnected (class ClientTcp * client)
{
    QString s = QString("ServerTcp [порт %1]. Отключен клиент %2").arg(port).arg(client->name());
    log(s);

    _clients.removeOne(client);                             // удаляем из списка клиентов
    emit disconnected(client);                              // уведомляем сервер
    delete client;                                          // удаляем выделенный экземпляр
}

// принята квитанция
void ServerTcp::slotRoger  (class ClientTcp *client)
{
    emit (roger(client));
}

// отправка данных "как есть" всем клиентам
void ServerTcp::sendToAll(char * data, quint16 length, bool rqAck)
{
    foreach (ClientTcp * client, _clients)
    {
        try
        {
            if (client->isConnected() && (client->isAcked() || !rqAck))
                client->send(data, length);
        }
        catch (...)
        {

        }
    }
}

void ServerTcp::packsendToAll(char * data, quint16 length, bool compress)
{
    SignaturedPack pack((char*)data, length, compress);
    sendToAll((char*)&pack, pack.length);
}
