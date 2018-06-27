#ifndef SERVERTCP_H
#define SERVERTCP_H

#include <QTcpServer>
#include <QTextCodec>
#include "clienttcp.h"
#include "logger.h"

class ServerTcp: public QObject
{
    Q_OBJECT
public:
    ServerTcp(quint16 port, QHostAddress bind=QHostAddress::Any, Logger * = nullptr); // конструктор получает порт и, возможно, интерфейс привязки
    ~ServerTcp();

    QList <class ClientTcp*> clients() { return _clients; }
    void sendToAll(char * data, quint16 length, bool rqAck=false);
    void packsendToAll(char * data, quint16 length, bool compress=false);
    void start();
signals:
    void	newConnection(class ClientTcp *);               // подключение нового клиента
    void    dataready   (class ClientTcp *);                // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void	acceptError (class ClientTcp *);                // ошибка на сокете
    void    disconnected (class ClientTcp *);               // разрыв соединения
    void    roger        (class ClientTcp *);               // принята квитанция

private slots:
    void	slotNewConnection();
    void    slotDataready    (class ClientTcp *);           // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void    slotRawdataready (class ClientTcp *);           // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void    slotDisconnected (class ClientTcp *);           // разрыв соединения
    void	slotAcceptError  (class ClientTcp *);           // ошибка на сокете
    void    slotRoger        (class ClientTcp *);           // принята квитанция

private:
    QList <class ClientTcp*> _clients;
    QTcpServer *tcpServer;
    quint16 port;
    QHostAddress bind;
    Logger      * logger;                                   // логгер для протоколирования
    QString     msg;                                        // строка для формирования сообщений

    void log (QString&);
};

#endif // SERVERTCP_H
