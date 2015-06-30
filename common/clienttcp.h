#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include "tcpheader.h"

class ClientTcp
{
    Q_OBJECT

// слоты для подключения к событиям QTcpSocket
public slots:
    void slotReadyRead      ();
    void slotConnected      ();
    void slotDisconnected   ();
    void slotError (QAbstractSocket::SocketError);

// сигналы, для подключения слотов вызывающего модуля
signals:
    void connected   (ClientTcp *);
    void disconnected(ClientTcp *);
    void error       (ClientTcp *);
    void dataready   (ClientTcp *);

public:
    ClientTcp(QString ipport);
    ClientTcp(QString ip, int port);
    ~ClientTcp(QString ip);

    void bind (QString ip);
    void compressMode(bool s);

private:
    QTcpSocket * sock;
    QString     bindIP;
    bool        compress;
    char        data[65536+8];
};

#endif // CLIENTTCP_H
