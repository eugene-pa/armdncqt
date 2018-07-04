#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "QHostAddress"
#include "QHostInfo"
#include "QNetworkInterface"
#include "tcpheader.h"

TcpHeader::TcpHeader()
{
    signature = SIGNATURE;
    length = 4;
}

TcpHeaderExt::TcpHeaderExt()
{
    signature = SIGNATURE;
    signature2 = 0xffff;
    length = 8;
}

// функции qCompress/qUncompress совместимы с используемым в С++/C# проектах форматом ZLIB с особенностями:
// - сжатые данные включают 6-байтный префикс:
// - 4 байта оригинальной длины (похоже, не используются, можно проставить нули)
// - 2 байта сигнатуры ZIP 0x78,0x9C, совпадающие с сигнатурой библиотеки ZLIB
SignaturedPack::SignaturedPack(char * src, int length, bool compress)
{
    pack(src, length, compress);
}

SignaturedPack::SignaturedPack(QByteArray& array, bool compress)
{
    pack(array.data(), array.length(), compress);
}

void SignaturedPack::pack(char * src, int srclength, bool compress)
{
    memset(data, 0, sizeof(data));
    if (compress)
    {
        QByteArray srcarr(srclength, 0);
        for (int i=0; i<srclength; i++)
            srcarr[i] = src[i];
        QByteArray zipped = qCompress(srcarr);              // можно менять степень сжатия!
        memcpy(data, zipped.data() + 4, srclength = zipped.length()-4);
    }
    else
        memcpy(data, src, srclength);
    signature = compress ? SIGNATUREZIP : SIGNATURE;        //signature = SIGNATURE;
    length = (WORD)(sizeof(TcpHeader) + srclength);
}


SignaturedPackExt::SignaturedPackExt(char * src, int length, bool compress)
{
    if (compress)
        signature = SIGNATUREZIP;
    pack(src, length, compress);
}

SignaturedPackExt::SignaturedPackExt(QByteArray& array, bool compress)
{
    if (compress)
        signature = SIGNATUREZIP;
    pack(array.data(), array.length(), compress);
}

void SignaturedPackExt::pack(char * src, int srclength, bool compress)
{
    memset(data, 0, sizeof(data));
    if (compress)
    {
        QByteArray srcarr(srclength, 0);
        for (int i=0; i<srclength; i++)
            srcarr[i] = src[i];
        QByteArray zipped = qCompress(srcarr);
        memcpy(data, zipped.data() + 4, srclength = zipped.length()-4);
    }
    else
        memcpy(data, src, srclength);

    signature = compress ? SIGNATUREZIP : SIGNATURE;
    signature2 = 0xffff;

    length = sizeof(TcpHeaderExt) + srclength;
}


QString TcpHeader::ErrorInfo (QAbstractSocket::SocketError error)
{
    switch (error)
    {
        case QAbstractSocket::ConnectionRefusedError            : return "Нет соединения с удаленным хостом";
        case QAbstractSocket::RemoteHostClosedError             : return "Удаленный хост разорвал соединение";
        case QAbstractSocket::HostNotFoundError                 : return "Адрес узла не найден";
        case QAbstractSocket::SocketAccessError                 : return "Не достаточно прав для операции на сокете";
        case QAbstractSocket::SocketResourceError               : return "Не достаточно системных ресурсов";
        case QAbstractSocket::SocketTimeoutError                : return "Истекло время для операции с сокетом";
        case QAbstractSocket::DatagramTooLargeError             : return "Размер дейтаграммы превышаеи максимальный";
        case QAbstractSocket::NetworkError                      : return "Ошибка сетевого оборудования";
        case QAbstractSocket::AddressInUseError                 : return "Адрес привязки уже используется в эксклюзивном режиме";
        case QAbstractSocket::SocketAddressNotAvailableError	: return "Адрес привязки не найден на узле";
        case QAbstractSocket::UnsupportedSocketOperationError	: return "Запрашиваемая операция не поддерживается ОС";
        case QAbstractSocket::ProxyAuthenticationRequiredError	: return "Сокет использует прокси, который запрашивает аутентификацию";
        case QAbstractSocket::UnknownSocketError                :
        default                                                 : return "Неопределённая ошибка сокета";
    }
}

bool TcpHeader::ParseIpPort(QString& ipport, QString& ip, int& port)
{
    bool ret = true;
    ip.clear();
    port = 0;
    QRegularExpressionMatch match = QRegularExpression("\\b([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3}):[0-9]{1,5}\\b").match(ipport);
    if (match.hasMatch())
    {
        // проблема: обработка рекурсивных подключений, например: 127.0.0.1:28080/192.168.0.101:28080
        QString lexem = match.captured();
        QRegularExpressionMatch matchip   = QRegularExpression(".+(?=:)").match(lexem);
        QRegularExpressionMatch matchport = QRegularExpression("(?<=:).+").match(lexem);
        // QRegularExpressionMatch matchport2 = QRegularExpression("\\d+$").match(ipport);
        if (matchip.hasMatch())
            ip = matchip.captured();
        else
            ret = false;
        if (matchport.hasMatch())
            port = matchport.captured().toInt();
        else
            ret = false;
    }
    else
        ret = false;
    return ret;
}

// глобальная функция: получить IP-адрес локального хоста, по возможности, реального сетевого адаптера
// возвращает первый IP из списка адресов QHostAddress, удовлетворяющий рег.выражению и не равный "127.0.0.1"
// если не нашли - возвращаем "127.0.0.1"
QString GetHostIp()
{
    QList< QHostAddress > addresses = QNetworkInterface::allAddresses();

    foreach ( const QHostAddress & a, addresses )
    {
        if ( a.protocol() == QAbstractSocket::IPv4Protocol && a.toString() != "127.0.0.1")
        {
            QRegularExpressionMatch match = QRegularExpression("\\b([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\b").match(a.toString());
            if (match.hasMatch())
                return a.toString();
        }
    }
    return "127.0.0.1";
}
