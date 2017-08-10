#ifndef RSPROXY_H
#define RSPROXY_H
#include <thread>											// std::thread
#include <iostream>											// std::cout
#include <queue>											// FIFO
#include <cstdio>
#include <mutex>											// мьютексы
#include <condition_variable>                               // std::condition_variable
#include <QSerialPort>
#include <QSerialPortInfo>
//#include <QThread>

//#define CONSOLAPP                                         // определить для консольного приложения

class RsProxy : public QSerialPort
{
public:
    RsProxy(QString  settings);                             // COM1,9600,N,8,1
    int GetCh(int ms=0);                                    // получить символ с ожиданием не более ms миллисекунд
    unsigned char GetCh(bool& ret, int ms=0);               // перегруженная функция; ret = true - прочитали.

    bool Send (const char *, int length);                   // передача массива данных заданной длины
    bool Send (QByteArray& data);
    void Close();                                           // завершение работы
    void SetScanDelay (int t) { ScanDelay = t; }            // задать периодичность сканирования входных данных

private:
    int  ScanDelay;                                         // периодичность сканирования входных данных, мс
    std::thread * pThreadRead;								// указатель на поток чтения
//    QThread ReadingThread;
    std::mutex mtxBuf;                                      // блокировка доступа к очереди
    std::mutex mtxWater;                                    // мьютекс для организации ожидания поступления данных
    std::condition_variable water;                          // условие ожидание приема данных

    QString settings;                                       // настройки,например: COM1,9600,N,8,1
    QString name;                                           // имя порта
    qint32 baudRate;                                        // скорость
    QSerialPort::Parity parity;                             // четность
    QSerialPort::DataBits dataBits;                         // бит данных
    QSerialPort::StopBits stopBits;                         // число стоп-бит
    
    std::deque<unsigned char> buffer;                       // кольцевой буфер данных
    std::size_t maxSize;                                    // максимально допустимая длина невыбранных данных, после которой при приеме удаляются самые старые данные

    bool parse(QString);                                    // разбор строки типа "COM1,9600,N,8,1"
    bool parsed;
    static void ThreadReading (long);						// функция чтения данных
    bool rqExit;                                            // завершение работы
};

#endif // RSPROXY_H
