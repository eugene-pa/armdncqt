#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include "tcpheader.h"


// класс для работы с форматированными пакетами межсетевого протокола ДЦ "ЮГ"
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
    ClientTcp(QString& ipport);
    ClientTcp(QString& ip, int port);
    ~ClientTcp();

    void bind (QString& ip);
    void compressMode(bool s);

private:
    QString     ip;
    int         port;
    QTcpSocket * sock;                                      // сокет
    QString     bindIP;                                     // IP привязки или пустая строка
    bool        compress;                                   // сжатие на лету
    char        * data;                                     // указатель на данные

    void        init();
};

#endif // CLIENTTCP_H
