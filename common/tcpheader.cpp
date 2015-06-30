#include "tcpheader.h"




QString TcpHeader::ErrorInfo (QAbstractSocket::SocketError error)
{
    switch (error)
    {
        case QAbstractSocket::ConnectionRefusedError:
            return "Соединение было разорвано другим узлом (или по тайм-ауту)";

        case QAbstractSocket::RemoteHostClosedError             : return "Удалённый хост закрыл соединение. Помните, что сокет клиента (например, текущий сокет) будет закрыт после отправления удалённого уведомления";
        case QAbstractSocket::HostNotFoundError                 : return "Адрес узла не найден";
        case QAbstractSocket::SocketAccessError                 : return "Операция с сокетом была прервана, так как приложение не получило необходимых прав";
        case QAbstractSocket::SocketResourceError               : return "У текущей системы не хватило ресурсов (например, слишком много сокетов)";
        case QAbstractSocket::SocketTimeoutError                : return "Время для операции с сокетом истекло";
        case QAbstractSocket::DatagramTooLargeError             : return "Дейтаграмма больше, чем установленное ограничение в операционной системе (которое может быть меньше, чем 8192 байт)";
        case QAbstractSocket::NetworkError                      : return "Произошла ошибка в сети (например, сетевой кабель был неожиданно отключён)";
        case QAbstractSocket::AddressInUseError                 : return "Адрес, определённый в QUdpSocket::bind(), уже используется и установлен в состояние эксклюзивного использования";
        case QAbstractSocket::SocketAddressNotAvailableError	: return "Адрес, определённый в QUdpSocket::bind(), не найден на узле";
        case QAbstractSocket::UnsupportedSocketOperationError	: return "Запрашиваемая операция с сокетом не поддерживается текущей операционной системой (например, отсутствует поддержка IPv6)";
        case QAbstractSocket::ProxyAuthenticationRequiredError	: return "Сокет использует прокси, который запрашивает аутентификацию";
        case QAbstractSocket::UnknownSocketError                :
        default                                                 : return "Произошла неопределённая ошибка";
    }
}

