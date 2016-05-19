#ifndef SERVERTCP_H
#define SERVERTCP_H

#include <QTcpServer>
#include "clienttcp.h"
#include "logger.h"

class ServerTcp: public QObject
{
    Q_OBJECT
public:
    ServerTcp(quint16 port, QHostAddress bind=QHostAddress::Any, Logger * = nullptr); // конструктор получает порт и, возможно, интерфейс привязки
    ~ServerTcp();

    void sendToAll(char * data, quint16 length);

private slots:
    void	acceptError(QAbstractSocket::SocketError socketError);
    void	newConnection();
    void dataready   (class ClientTcp *);                   // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(class ClientTcp *);                   // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены

private:
    QList <class ClientTcp*> clients;
    QTcpServer *tcpServer;
    quint16 port;
    QHostAddress bind;
    Logger      * logger;                                   // логгер для протоколирования
    QString     msg;                                        // строка для формирования сообщений

    void log (QString&);
};

#endif // SERVERTCP_H
