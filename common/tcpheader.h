#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QTcpSocket>
#include <QAbstractSocket>
#include "defines.h"

class TcpHeader
{
public:
    bool Signatured() { return signature == SIGNATURE; }    // данные форматированы?
    bool Extended  () { return length    == 0xffff;    }    // формат расширенный  ?
    static QString ErrorInfo (QAbstractSocket::SocketError error);  // текстовая информация об ошибке
    static bool ParseIpPort(QString& ipport, QString& ip, QString& port);
private:
    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    length;                                         // если FFFF - расширенный пакет, следующие 3 байта - длина
};




#endif // TCPHEADER_H
