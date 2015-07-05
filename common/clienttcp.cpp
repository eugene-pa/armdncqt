#include <QTextCodec>
#include "clienttcp.h"


ClientTcp::ClientTcp(QString& ip, int port, Logger * p, bool compress, QString idtype)
{
    this->ip = ip;
    this->port = port;
    logger = p;
    this->compress = compress;
    this->idtype = idtype;
    init();
}

ClientTcp::ClientTcp(QString& ipport, Logger * p, bool compress, QString idtype)
{
    TcpHeader::ParseIpPort(ipport, ip, port);
    logger = p;
    this->compress = compress;
    this->idtype = idtype;
    init();
}

void ClientTcp::init()
{
    sock = new QTcpSocket();
    run = false;
    data = new char[65536 + 8];
    toRead = sizeof(TcpHeader);                             // 4 байта - чтение заголовка
    length = 0;
    rcvd[0] = rcvd[1] = sent[0] = sent[1] = 0;

    // привязка своих слотов к сигналам QTcpSocket
    QObject::connect(sock, SIGNAL(connected   ()), this, SLOT(slotConnected()));
    QObject::connect(sock, SIGNAL(readyRead   ()), this, SLOT(slotReadyRead()));
    QObject::connect(sock, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    QObject::connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));

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
        length += sock->read(data+length, toRead-length);
        if (length < toRead)
        {
            qDebug() << "Недобор до" << toRead << ". Прочитано: " << length;
            return;
        }
        if (length==sizeof(TcpHeader))
        {
            if (((TcpHeader *)data)->Signatured())
            {
                qDebug() << "Сигнатура";
                toRead = ((TcpHeader *)data)->Length();     // общая длина пакета (загловок + данные)
            }
            else
            {
                qDebug() << "Неформатные данные";
                // читаем все, что есть
                length += sock->read(data+length, 65536-length);
                rcvd[1] += length;                          // инкремент

                rawdataready(this);
                return;
            }
        }
        else
        {
            rcvd[0]++; rcvd[1] += length;                   // инкремент
            dataready (this);                               // обращение к подключенным слотам; они должны гарантированно забрать данные

            length = 0;
            toRead = sizeof(TcpHeader);
        }
    }
}

// установлено соединение
void ClientTcp::slotConnected ()
{
    log (msg=QString("Установлено соединения c хостом %1:%2").arg(ip).arg(port));

    // если определен тип, отправляет тип удаленному серверу, преобразовав в кодировку Windows-1251
    if (idtype.length())
    {
        QByteArray id = QTextCodec::codecForName("Windows-1251")->fromUnicode(idtype);
        id[id.length()] = 0;
        Send(id);
    }
    connected (this);
}

// разорвано соединение
void ClientTcp::slotDisconnected ()
{
    log (msg=QString("Разрыв соединения c хостом %1").arg(Name()));
    disconnected (this);
    if (run)
        sock->connectToHost(ip,port);
}

// ошибка
void ClientTcp::slotError (QAbstractSocket::SocketError er)
{
    _lasterror = er;
    log (msg=QString("Клиент %1. Ошибка: %2").arg(Name()).arg(TcpHeader::ErrorInfo(er)));
    error (this);
//  if (run && !connected())
//      sock->connectToHost(ip,port);
}

void ClientTcp::log (QString& msg)
{
    if (logger != nullptr)
        logger->log(msg);
    else
        qDebug() << msg;
}

// передача блока данных
void ClientTcp::Send(void * p, int length)
{
    if (connected())
    {
        sock->write((char*)p,length);
        sent[0]++; sent[1] += length;
    }
    else
        log (msg=QString("Игнорируем отправку данных в разорванное соединение %1").arg(Name()));
}

// передача массива
void ClientTcp::Send(QByteArray& array)
{
    if (connected())
    {
        sock->write(array);
        sent[0]++; sent[1] += array.length();
    }
    else
        log (msg=QString("Игнорируем отправку данных в разорванное соединение %1").arg(Name()));
}

void ClientTcp::SendAck()
{
    TcpHeader ack;
    Send (&ack, sizeof(ack));
}
