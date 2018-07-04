#ifndef TCPHEADER_H
#define TCPHEADER_H

#include <QTcpSocket>
#include <QAbstractSocket>
#include "defines.h"

const int maxSize = 65536 + 8;

// класс заголовка
class TcpHeader
{
public:
    // статические функции
    static QString ErrorInfo (QAbstractSocket::SocketError error);      // текстовая информация об ошибке
    static bool ParseIpPort(QString& ipport, QString& ip, int& port);   // разбор строки IP:порт

    TcpHeader();                                                        // конструктор

    // ввожу отдельно поняие сигнатуры сжатого пакета
    bool Signatured   () { return signature == SIGNATURE || signature == SIGNATUREZIP; } // данные форматированы?
    bool SignaturedZip() { return signature == SIGNATUREZIP; }                          // данные сжаты?
    bool Extended  () { return length    == 0xffff;    }                // формат расширенный  ?
    int Length() { return length; }

private:
    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    length;                                         // общая длина пакета (загловок + данные)
                                                            // если FFFF - расширенный пакет, следующие 3 байта - длина
};

// расширенный класс заголовка с поддержкой пакетов длиной более 65535
class TcpHeaderExt
{
    // Длина заголовка - 8 байт
    // Заголовок: 55 АА FF FF - признак расширенного формата
    // Длина    : XX XX XX
    // Резерв   : ХХ
public:
    TcpHeaderExt();
    bool Signatured() { return signature == SIGNATURE && signature2 == 0xffff; } // формат расширенный  ?
    int Length() { return length; }
protected:
    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    signature2;                                     // SIGNATURE 0xFFFF
    UINT    length;                                        // общая длина пакета (загловок + данные)
};

class SignaturedPack
{
public:
    SignaturedPack(char * data, int length, bool compress=false);
    SignaturedPack(QByteArray& data, bool compress=false);
    void pack(char * data, int length, bool compress=false);
//protected:
    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    length;                                         // общая длина пакета (загловок + данные)
    char data[maxSize];
};

class SignaturedPackExt
{
public:
    SignaturedPackExt(char * data, int length, bool compress=false);
    SignaturedPackExt(QByteArray& data, bool compress=false);
    void pack(char * data, int length, bool compress=false);

    WORD    signature;                                      // SIGNATURE 0xAA55
    WORD    signature2;                                     // SIGNATURE 0xFFFF
    UINT    length;                                         // общая длина пакета (загловок + данные)
    char data[maxSize];
};

QString GetHostIp();                                        // глобальная функция: получить IP-адрес локального хоста, по возможности, реального сетевого адаптера

#endif // TCPHEADER_H
