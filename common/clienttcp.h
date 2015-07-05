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
    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

public:
    ClientTcp(QString& ipport, Logger * = nullptr, bool compress=false, QString idtype = "");
    ClientTcp(QString& ip, int port, Logger * = nullptr, bool compress=false, QString idtype = "");
    ~ClientTcp();

    void start ();                                          // старт работы сокета
    void stop  ();                                          // останов сокета
    void bind (QString& ip);
    void compressMode(bool s);
    void setlogger(Logger * p) { logger = p; }
    void setid(QString id) { idtype = id; }
    bool connected() { return sock->state() == QAbstractSocket::ConnectedState; }
    QAbstractSocket::SocketError lasterror() { return _lasterror; }

    QString Name()                                          // имя в формате IP:порт
        { return QString("%1:%2").arg(ip).arg(port); }
    char * RawData()    { return data; }                    // "сырые данные" - полный пакет с заголовком
    int    RawLength()  { return length; }                  // длина пакета с заголовком
    char * Data()       { return data + sizeof(TcpHeader); }// пользовательские данные
    int    Length()     { return length - sizeof(TcpHeader); } // длина пользовательских данных
    void   Send(void *data, int length);                    // передача
    void   Send(QByteArray&);                               // передача
    void   SendAck();                                       // квитанция
    UINT   Rcvd(int i) { return rcvd[i?1:0]; }              // счетчик: 0-пакетов, 1-байтов
    UINT   Sent(int i) { return sent[i?1:0]; }              // счетчик: 0-пакетов, 1-байтов

private:
    QString     ip;
    int         port;
    QString     idtype;                                     // идентификатор типа клиента
    QTcpSocket  * sock;                                     // сокет
    QString     bindIP;                                     // IP привязки или пустая строка
    char        * data;                                     // указатель на данные
    bool        run;                                        // старт/стоп
    QString     msg;                                        // строка для формирования сообщений
    Logger      * logger;                                   // логгер для протоколирования
    bool        compress;                                   // сжатие на лету
    QAbstractSocket::SocketError _lasterror;                // ошибка

    // состояние приема пакета
    int         toRead;                                     // требуемый объем данных
    int         length;                                     // прочитанный объем данных
    UINT        rcvd[2];                                    // получено   (пакеты, байты)
    UINT        sent[2];                                    // отправлено (пакеты, байты)

    void init ();
    void log (QString&);
};

#endif // CLIENTTCP_H
