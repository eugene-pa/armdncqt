#ifndef BLOCKINGRS_H
#define BLOCKINGRS_H

#include <QSerialPort>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "defines.h"

// BlockingRs - Базовый класс для создания производных классов, реализующих
// тот или иной протокол обмена по последовательному асинхронному интерфейсу
// BlockingRs наследуется от QThread и работает с последовательным портом
// с помощью класса QSerialPort в блокирующем режиме
// ПОРЯДОК ИСПОЛЬЗОВАНИЯ В ПРОЕКТАХ:
// - включить в проект файлы blockingrs.h, blockingrs.cpp
// - создать клас, производный от blockingrs (см. в примере class RasRS : public BlockingRs)
// - переопределить вирт.функцию mainLoop(), реализующую логику и последовательность протокола обмена
// - производный класс создается в нужном классе основного потока приложения с увязкой сигналов и слотов:
//   (в примере см. функцию MainWindow::on_action_Start_triggered()), после чего запускается раб.поток
//   функцией startRs, в каческте параметров которой передаются конфигурация порта и тайм-ауты
//
// СИГНАЛЫ
// dataready(QByteArray)    - приняты данные
// timeout()                - нет данных
// error(int)               - ошибка с кодом ошибки
// started()                - старт потока
//
// СЛОТЫ:
// exit()                   - завершение работы


class BlockingRs : public QThread
{
    Q_OBJECT
public:
    BlockingRs(QObject *parent = 0, BYTE marker=0, int maxlength=4048);
    ~BlockingRs();

    enum DataError
    {
        TIMEOUT     = 0x81,                                 // тайм аут
        CRC         = 0x82,                                 // несовпадение CRC
        L_OVER      = 0x83,                                 // превышен размер
        FORMAT      = 0x84,                                 // нарушен формат
        TRASH       = 0x85,                                 // "мусор" в канале
        UNAVAILABLE = 0x86,                                 // ошибка открытия порта
    };

    void startRs(const QString &settings, COMMTIMEOUTS tm);
    void run();
    QString& name() { return _name; }
    static QString errorText (DataError error);
    static QString errorText (int error);

    // виртуальные функции
    virtual void mainLoop();                                // основной цикл
    virtual void doData(QByteArray&);                       // можно перекрыть

    QString errorText() { return lastErrorText; }

private slots:
    void exit();

signals:
    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void error    (int);                                    // сигнал-уведомление об ошибке
    void timeout  ();                                       // сигнал-уведомление о таймауте

protected:
    virtual QByteArray readData(QSerialPort& serial);       // прием пакета данных с заданными таймаутами, маркером и максимальной длиной
    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"

    QString lastErrorText;
    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString _name;                                          // имя порта
    qint32 baudRate;                                        // скорость
    QSerialPort::Parity parity;                             // четность
    QSerialPort::DataBits dataBits;                         // бит данных
    QSerialPort::StopBits stopBits;                         // число стоп-бит

    QSerialPort * pSerial;
    QMutex mutex;
    bool quit;
    COMMTIMEOUTS tm;
    bool parsed;

    QObject * parent;                                       // родитель
    BYTE marker;                                            // маркер или 0
    int maxlength;                                          // максимальная длина
//    QSerialPort serial;

    bool FindMarker (QByteArray&, BYTE marker=0);
};

#endif // BLOCKINGRS_H
