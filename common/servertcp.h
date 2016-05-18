#ifndef SERVERTCP_H
#define SERVERTCP_H

#include <QTcpServer>

class ServerTcp: public QObject
{
    Q_OBJECT
public:
    ServerTcp(quint16 port, QHostAddress& bind=QHostAddress::Any); // конструктор получает порт и, возможно, интерфейс привязки
    ~ServerTcp();

private slots:
    void	acceptError(QAbstractSocket::SocketError socketError);
    void	newConnection();
private:
    QTcpServer *tcpServer;
};

#endif // SERVERTCP_H
