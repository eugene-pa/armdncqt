#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QTcpSocket>
#include <QAbstractSocket>
#include "defines.h"

class TcpHeader
{
public:
    // статические функции
    static QString ErrorInfo (QAbstractSocket::SocketError error);      // текстовая информация об ошибке
    static bool ParseIpPort(QString& ipport, QString& ip, int& port);   // разбор строки IP:порт

    TcpHeader();                                                        // конструктор

    bool Signatured() { return signature == SIGNATURE; }                // данные форматированы?
    bool Extended  () { return length    == 0xffff;    }                // формат расширенный  ?
    int Length() { return length; }

private:
    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    length;                                         // общая длина пакета (загловок + данные)
                                                            // если FFFF - расширенный пакет, следующие 3 байта - длина
};




#endif // TCPHEADER_H
