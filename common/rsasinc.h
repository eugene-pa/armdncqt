#ifndef RSASINC_H
#define RSASINC_H

#include <iostream>											// std::cout
#include <queue>											// FIFO
#include <cstdio>
#include <mutex>											// мьютексы
#include <condition_variable>                               // std::condition_variable

#include <QObject>
#include <QSerialPort>

#define CONSOLAPP                                           // определить для консольного приложения

class RsAsinc : public QObject                              // наследуемся от QObject для поддержки сигналов и слотов
{
    Q_OBJECT

public:
    RsAsinc(QString  settings);                             // Конструктор, на входе строка типа "COM1,9600,N,8,1"

    int  GetCh();                                           // получить символ с ожиданием не более timeWaiting миллисекунд
    int  GetChEx();                                         // получить символ с ожиданием не более timeWaiting миллисекунд
    int  GetCh(int ms);                                     // получить символ с ожиданием не более ms миллисекунд
    bool Send (const char *, int length);                   // передача массива char заданной длины
    bool Send (QByteArray& data);                           // передача массива QByteArray
    void SetTimeWaiting(int ms) { timeWaiting = ms; }       // изменение времени ожидания по умолчанию
    QSerialPort& Serial() { return serial; }                // доступ непосредственно к классу QSerialPort
    bool Parsed() { return parsed; }                        // проверка успешности парсинга параметров порта
    bool CourierDetect();                                   // проверка несущей
    bool IsOpen() { return serial.isOpen(); }               // проверка 'порт открыт'?
    void Close();                                           // завершение работы

private slots:
    void readData();                                        // уведомление о готовности данных по приему
    void handleError(QSerialPort::SerialPortError);         // уведомление об ошибке

private:
    QSerialPort serial;                                     // экземпляр QSerialPort

    std::mutex mtxBuf;                                      // блокировка доступа к очереди
    std::mutex mtxWater;                                    // мьютекс для организации ожидания поступления данных
    std::condition_variable water;                          // условие ожидание приема данных

    int timeWaiting;                                        // время ожидания символа по умолчанию, мс

    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString name;                                           // имя порта

    std::deque<unsigned char> buffer;                       // очередь FIFO принятых данных
    std::size_t maxSize;                                    // максимально допустимая длина невыбранных данных,
                                                            // после которой при приеме удаляются самые старые данные
    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"
    bool parsed;                                            // true - описание порта корректное
    bool rqExit;                                            // завершение работы
};


#endif // RSASINC_H
