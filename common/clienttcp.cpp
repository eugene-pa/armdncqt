#include "clienttcp.h"


ClientTcp::ClientTcp(QString& ip, int port, Logger * p, bool compress)
{
    this->ip = ip;
    this->port = port;
    logger = p;
    this->compress = compress;
    init();
}

ClientTcp::ClientTcp(QString& ipport, Logger * p, bool compress)
{
    TcpHeader::ParseIpPort(ipport, ip, port);
    logger = p;
    this->compress = compress;
    init();
}

void ClientTcp::init()
{
    sock = new QTcpSocket();
    run = false;
    data = new char[65536 + 8];
    toRead = sizeof(TcpHeader);                             // 4 байта - чтение заголовка
    length = 0;
    log(msg = QString("Конструктор ClientTcp %1:%2").arg(ip).arg(port));
}

ClientTcp::~ClientTcp()
{
    log(msg = QString("Деструктор ClientTcp: %1:%2").arg(ip).arg(port));
    sock->abort();
    sock->close();
    delete data;
}

void ClientTcp::start()
{
    if (!run)
    {
        run = true;
        sock->connectToHost(ip,port);
    }
}

void ClientTcp::stop()
{
    run = false;
    if (connected())
        sock->disconnectFromHost();
}

// прияем данных
void ClientTcp::slotReadyRead      ()
{
    while (true)
    {
        length += sock->read(data+length, toRead);
        if (length < toRead)
            return;
        if (length==4)
        {
            if (((TcpHeader *)data)->Signatured())
                toRead += ((TcpHeader *)data)->Length();
            else
            {
                // неформатные данные
                return;
            }
        }

    }
    dataready (this);
    length = 0;
    toRead = sizeof(TcpHeader);
}

// установлено соединение
void ClientTcp::slotConnected ()
{
    log (msg=QString("Установлено соединения c хостом %1:%2").arg(ip).arg(port));
    connected (this);
}

// разорвано соединение
void ClientTcp::slotDisconnected ()
{
    log (msg=QString("Разрыв соединения c хостом %1:%2").arg(ip).arg(port));
    disconnected (this);
    if (run)
        sock->connectToHost(ip,port);
}

// ошибка
void ClientTcp::slotError (QAbstractSocket::SocketError er)
{
    log (msg=QString("Клиент %1:%2. Ошибка: %3").arg(ip).arg(port).arg(TcpHeader::ErrorInfo(er)));
    error (this);
    if (run && !connected())
        sock->connectToHost(ip,port);
}

void ClientTcp::log (QString& msg)
{
    if (logger != nullptr)
        logger->log(msg);
    else
        qDebug() << msg;
}
