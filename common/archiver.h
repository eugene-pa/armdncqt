#ifndef ARCHIVER_H
#define ARCHIVER_H

#include "defines.h"

class ArchiveHeader                                         // заголовок архива
{
friend class ArhReader;
public:
    enum
    {
        arcTypeTS = 1,                                      // тип архива ТС
        arcTypeTU = 2,                                      // тип архива ТУ
        headersize = 12,
    };

    ArchiveHeader() { }                                     // конструктор по умолчанию
    ArchiveHeader(QFile * file);                            // конструктор на основе открытого и позиционированного файла
    int Read(QFile * file);                                 // чтение заголовка
    int Length() { return length; }
private:
    WORD	signature;                                      // сигнатура
    union
    {
        BYTE b[2];
        WORD w;
    } Rsrv;                                                 // резерв
    UINT    time;                                           // время time_t
    WORD	type;                                           // тип
    WORD	length;                                         // длина записи

    qint64  offset;                                         // смещение в файле относит.начала
};


// класс для чтения архива
// дилемма: сканировать файл или нет
// для чтения архива в режиме АРМ ШН можно не сканирвать, для построения анализаторов по типу ArhViewer - сканировать
// можно попытаться сделать двойной режим (сканировать только по запросу) и выбирать режим в конкретном приложении
class ArhReader
{
public:
    ArhReader(QString& filename);                           // конструктор для чтения
    ~ArhReader();
    int Next ();                                            // чтение записи от текущего положения указателя
    int First();                                            // чтение первой записи
    int Last ();                                            // чтение последней записи
    int Prev ();                                            // чтение предыдущей записи
    int Read (UINT);                                        // чтение записи по дате >= заданной
    int Read (int);                                         // чтение записи по индексу (номеру-1)

    char * Data() { return data; }
    int  Length() { return header.Length(); }
private:
    ArchiveHeader header;                                   // заголовок
    QDir    dir;                                            // папка для чтения
    QString filename;                                       // файл для чтения
    QString prefix;                                         // префикс при формировании имени файла для записи (пример @_)
    QString ext;                                            // расширение имени
    QFile   file;                                           // актуальный открытый файд

    char    data[65535];                                    // буфер под максимальный размер данных
};


// класс для записи архива
class ArhWriter
{
public:
    ArhWriter(QString& dir, QString& prefix);               // конструктор для записи
private:
    QDir    dir;                                            // папка для записи
    QString filename;                                       // файл для записи
    QString prefix;                                         // префикс при формировании имени файла для записи (пример @_)
};

#endif // ARCHIVER_H