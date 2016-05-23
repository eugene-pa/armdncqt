#include <QTextCodec>
#include "clienttcp.h"


ClientTcp::ClientTcp(ServerTcp *server, QTcpSocket  *sock, Logger * logger)  // конструктор, используемый сервером
{
    this->server = server;
    this->sock = sock;
    this->logger = logger;

    // прописываем IP адрес клиента и локальный порт
    remoteIp = sock->peerAddress().toString();
    remotePort = sock->localPort();

    init();
}

ClientTcp::ClientTcp(QString& ip, int port, Logger * p, bool compress, QString idtype)
{
    server = nullptr;
    this->remoteIp = ip;
    this->remotePort = port;
    logger = p;
    this->compress = compress;
    this->idtype = idtype;
    sock = new QTcpSocket();
    log(msg = QString("Конструктор ClientTcp %1:%2").arg(remoteIp).arg(remotePort));

    init();
}

ClientTcp::ClientTcp(QString& ipport, Logger * p, bool compress, QString idtype)
{
    server = nullptr;
    TcpHeader::ParseIpPort(ipport, remoteIp, remotePort);
    logger = p;
    this->compress = compress;
    this->idtype = idtype;
    sock = new QTcpSocket();
    log(msg = QString("Конструктор ClientTcp %1:%2").arg(remoteIp).arg(remotePort));

    init();
}

void ClientTcp::init()
{
    _userPtr = nullptr;
    run = false;
    _data = new char[maxSize];
    clear();

    toRead = sizeof(TcpHeader);                             // 4 байта - чтение заголовка
    _length = 0;
    rcvd[0] = rcvd[1] = sent[0] = sent[1] = 0;

    // привязка своих слотов к сигналам QTcpSocket
    QObject::connect(sock, SIGNAL(connected   ()), this, SLOT(slotConnected()));
    QObject::connect(sock, SIGNAL(readyRead   ()), this, SLOT(slotReadyRead()));
    QObject::connect(sock, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    QObject::connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(slotError(QAbstractSocket::SocketError)));
}

ClientTcp::~ClientTcp()
{
    log(msg = QString("Деструктор ClientTcp: %1:%2").arg(remoteIp).arg(remotePort));
    sock->abort();
    sock->close();
    delete _data;
}

void ClientTcp::start()
{
    if (!run)
    {
        run = true;
        if (!isServer())
            sock->connectToHost(remoteIp,remotePort);
    }
}

void ClientTcp::stop()
{
    run = false;
    if (isConnected())
        sock->disconnectFromHost();
}

// прием данных
// форматные данные:
// - WORD -  сигнатуры: 0xAA55
// - WORD -  длина пакета, включая заголовок
// - данные
//
// toRead - требуемая длина; если toRead==длине заголовка, принимаем заголовок, определяем длину пакета, и toRead = длине пакета
void ClientTcp::slotReadyRead      ()
{
    while (true)
    {
        _length += sock->read(_data+_length, toRead-_length);   // читаем в буфер со смещением length
        if (_length < toRead)
        {
            if (_length)
                qDebug() << "Недобор до" << toRead << ". Прочитано: " << _length;
            return;
        }
        if (_length==sizeof(TcpHeader))
        {
            // анализ первых 4-х байтов
            if (((TcpHeader *)_data)->Signatured())
            {
                // приняли заголовок пакета
                qDebug() << "Заголовок";
                toRead = ((TcpHeader *)_data)->Length();     // общая длина пакета (загловок + данные)
                if (toRead==4)
                {
                    rcvd[0]++; rcvd[1] += toRead;           // инкремент
                    qDebug() << "Квитанция";
                    acked = true;
                    return;
                }
            }
            else
            {
                // первые 4 - байта - не заголовок, значит, неформатные данные
                qDebug() << "Неформатные данные";
                // читаем все, что есть
                _length += sock->read(_data+_length, 65536-_length);
                rcvd[1] += _length;                          // инкремент

                emit rawdataready(this);

                _length = 0;
                toRead = sizeof(TcpHeader);

                return;
            }
        }
        else
        {
            // получили требуемую пачку данных (length >= toRead)
            rcvd[0]++; rcvd[1] += _length;                   // инкремент

            // распаковку делать не всегда, если это ретрансляция - можно не распаковывать, а передавать как есть
            uncompress();

            emit dataready (this);                          // обращение к подключенным слотам; они должны гарантированно забрать данные

            _length = 0;
            toRead = sizeof(TcpHeader);
        }
    }
}

// если данные упакованы - распаковать
// функции qCompress/qUncompress совместимы с используемым в С++/C# проектах форматом ZLIB с особенностями:
// - сжатые данные включают 6-байтный префикс:
// - 4 байта оригинальной длины (похоже, не используются, можно проставить нули)
// - 2 байта сигнатуры ZIP 0x78,0x9C, совпадающие с сигнатурой библиотеки ZLIB
//   С учетом того, что в сжатых пакетах длина заголовка пакета составляет 4 байта, за которым идет сигнатура ZIP 0x78,0x9C,
//   заголовок пакета используется как префикс
void ClientTcp::uncompress()
{
    if (isCompressed())
    {
        QByteArray zip(_data, _length);
//        zip[2] = 0;                                         // необязательные действия
//        zip[3] = 0;                                         // необязательные действия
        QByteArray unzip = qUncompress(zip);
        _length = unzip.length();                            // оригинальная длина
        _data[2] = (BYTE)_length;
        _data[3] = (BYTE)(_length >> 8);
        for (int i=0; i< _length; i++)                       // копируем данные
            _data[4+i] = unzip[i];
    }
}

// проверка префикса сжатых данных
bool ClientTcp::isCompressed()
{
    return _length > sizeof(TcpHeader) + 2 && (BYTE)_data[4] == 0x78 && (BYTE)_data[5] == 0x9C;
}

// установлено соединение
void ClientTcp::slotConnected ()
{
    log (msg=QString("Установлено соединения c хостом %1:%2").arg(remoteIp).arg(remotePort));

    // если определен тип, отправляет тип удаленному серверу, преобразовав в кодировку Windows-1251
    if (idtype.length())
    {
        QByteArray id = QTextCodec::codecForName("Windows-1251")->fromUnicode(idtype);
        id[id.length()] = 0;
        send(id);
    }
    emit connected (this);
}

// разорвано соединение
void ClientTcp::slotDisconnected ()
{
    log (msg=QString("Разрыв соединения c хостом %1").arg(name()));
    emit disconnected (this);
    if (run)
        sock->connectToHost(remoteIp,remotePort);
}

// ошибка
void ClientTcp::slotError (QAbstractSocket::SocketError er)
{
    _lasterror = er;
    log (msg=QString("Клиент %1. Ошибка: %2").arg(name()).arg(TcpHeader::ErrorInfo(er)));
    emit error (this);
    if (run && !isConnected())
        sock->connectToHost(remoteIp,remotePort);
}

void ClientTcp::log (QString& msg)
{
    if (logger != nullptr)
        logger->log(msg);
    else
        qDebug() << msg;
}

// передача блока данных
void ClientTcp::send(void * p, int length)
{
    if (isConnected())
    {
        sock->write((char*)p,length);
        sent[0]++; sent[1] += length;
        acked = false;
    }
    else
        log (msg=QString("Игнорируем отправку данных в разорванное соединение %1").arg(name()));
}

// передача массива
void ClientTcp::send(QByteArray& array)
{
    if (isConnected())
    {
        sock->write(array);
        sent[0]++; sent[1] += array.length();
        acked = false;
    }
    else
        log (msg=QString("Игнорируем отправку данных в разорванное соединение %1").arg(name()));
}

void ClientTcp::sendAck()
{
    TcpHeader ack;
    send (&ack, sizeof(ack));
}
