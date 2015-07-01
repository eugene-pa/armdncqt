#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include "tcpheader.h"
#include "logger.h"


// класс для работы с форматированными пакетами межсетевого протокола ДЦ "ЮГ"
class ClientTcp : public QObject
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
    ClientTcp(QString& ipport, Logger * = nullptr, bool compress=false);
    ClientTcp(QString& ip, int port, Logger * = nullptr, bool compress=false);
    ~ClientTcp();

    void start ();
    void stop  ();
    void bind (QString& ip);
    void compressMode(bool s);
    void setlogger(Logger * p) { logger = p; }
    bool connected() { return sock->state() == QAbstractSocket::ConnectedState; }
private:
    QString     ip;
    int         port;
    QTcpSocket  * sock;                                     // сокет
    QString     bindIP;                                     // IP привязки или пустая строка
    bool        compress;                                   // сжатие на лету
    char        * data;                                     // указатель на данные
    bool        run;                                        // старт/стоп
    QString     msg;                                        // строка для формирования сообщений
    Logger      * logger;                                   // логгер для протоколирования

    // состояние приема пакета
    int         toRead;                                     //
    int         length;

    void init ();
    void log (QString&);
};

#endif // CLIENTTCP_H
