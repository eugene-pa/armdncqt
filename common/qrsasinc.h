#ifndef QRSASINC_H
#define QRSASINC_H

#include <QObject>
#include "rsasinc.h"
#include "pasender.h"

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

const WORD LENGTH_OFFS		= 1;
const WORD DST_OFFS			= 3;
const WORD SRC_OFFS			= 4;
const WORD SEANS_OFFS		= 5;
const WORD FRAME_DATA_OFFS	= 6;

const int  LEN_CTRL_INFO= 6;								// длина обрамления (маркер, длина, CRC, EOT)
const int  LEN_HEADER	= 3;								// длина заголовка (маркер+поле длины)
const int  CRC_SIZE  	= 2;								//
const BYTE SOH			= 1;
const BYTE EOT			= 4;
const BYTE CpuAddress	= 0;								// адрес ЦПУ на линии

class QRsAsinc : public QObject
{
    Q_OBJECT
public:
    explicit QRsAsinc(QString ini, QObject *parent = nullptr);

public slots:
    void process();                                         // запуск рабочего цикла будущего потока

signals:
    void finished();                                        // сигнал завершения работы
    void error   ();                                        // сигнал ошибки

private:
    BYTE dataIn [4048];                                     // входные данные
    BYTE dataOut[4048];                                     // выходные данные

    RsAsinc * prs;                                          // указатель на рабочий RsAsing
    QString config;                                         // строка конфигурации
    std::function<void(PaSender&, class PaMessage *)> rsNotifier;
    int MakeData();
};

#endif // QRSASINC_H
