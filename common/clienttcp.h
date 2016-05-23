#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include "tcpheader.h"
#include "servertcp.h"
#include "logger.h"


// класс ClientTcp представляет собой обертку класса QTcpSocket, который участвует в TCP-соединении как на стороне клиента, так и на стороне сервера
// функция isServerConn() возвращает истину для серверного соединения

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
    const int maxSize = 65536 + 8;
    ClientTcp(class ServerTcp *, QTcpSocket  *, Logger *);  // конструктор, используемый сервером
    ClientTcp(QString& ipport, Logger * = nullptr, bool compress=false, QString idtype = "");
    ClientTcp(QString& remoteIp, int remotePort, Logger * = nullptr, bool compress=false, QString idtype = "");
    ~ClientTcp();

    void start ();                                          // старт работы сокета
    void stop  ();                                          // останов сокета
    void bind (QString& remoteIp);
    void compressMode(bool s);
    void setlogger(Logger * p) { logger = p; }
    void setid(QString id) { idtype = id; }
    QString getid() { return idtype; }
    bool isConnected() { return sock->state() == QAbstractSocket::ConnectedState; }
    QAbstractSocket::SocketError lasterror() { return _lasterror; }

    QString name()                                          // имя в формате IP:порт
        { return QString("%1:%2").arg(remoteIp).arg(remotePort); }
    char * rawData()    { return _data; }                    // "сырые данные" - полный пакет с заголовком
    int    rawLength()  { return _length; }                  // длина пакета с заголовком
    char * data()       { return _data + sizeof(TcpHeader); }// пользовательские данные
    int    length()     { return _length - sizeof(TcpHeader); } // длина пользовательских данных
    void   send(void *_data, int _length);                    // передача
    void   send(QByteArray&);                               // передача
    void   sendAck();                                       // квитанция
    UINT   getrcvd(int i) { return rcvd[i?1:0]; }           // счетчик: 0-пакетов, 1-байтов
    UINT   getsent(int i) { return sent[i?1:0]; }           // счетчик: 0-пакетов, 1-байтов
    void * userPtr(){ return _userPtr; }                    // получить пользовательский указатель
    void   setUserPtr(void *p) { _userPtr = p; }            // запомнить пользовательский указатель
    bool   isServer() { return server != nullptr; }         // это серверное соединение ?
    bool   isAcked () { return acked; }                     // квитировано ?
    void   clear() { memset(_data, 0, maxSize); }

private:
    ServerTcp   * server;                                   // владелец - сервер
    QString     remoteIp;
    int         remotePort;
    QString     idtype;                                     // идентификатор типа клиента
    QTcpSocket  * sock;                                     // сокет
    QString     bindIP;                                     // IP привязки или пустая строка
    char        * _data;                                     // указатель на данные (выделяем new char[65536 + 8];)
    bool        run;                                        // старт/стоп
    QString     msg;                                        // строка для формирования сообщений
    Logger      * logger;                                   // логгер для протоколирования
    bool        compress;                                   // сжатие на лету
    QAbstractSocket::SocketError _lasterror;                // ошибка
    void        * _userPtr;                                  // указатель на данные пользователя
    bool        acked;                                      // квитировано!

    // состояние приема пакета
    int         toRead;                                     // требуемый объем данных
    int         _length;                                     // прочитанный объем данных
    UINT        rcvd[2];                                    // получено   (пакеты, байты)
    UINT        sent[2];                                    // отправлено (пакеты, байты)

    void init ();
    void log (QString&);
    void uncompress();                                      // если данные упакованы - распаковать
    bool isCompressed();                                    // проверка префикса сжатых данных
};

#endif // CLIENTTCP_H
