#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "tcpheader.h"


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
        case QAbstractSocket::NetworkError                      : return "Ошибка сетевого оборудованияв";
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
        QRegularExpressionMatch matchip   = QRegularExpression(".+(?=:)").match(ipport);
        QRegularExpressionMatch matchport = QRegularExpression("(?<=:).+").match(ipport);
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
