#ifndef LOGGER_H
#define LOGGER_H

#include <QDir>
#include <QMutex>

class Logger
{
private:
    QDir    dir;                                                // каталог
    QString sFileNameWithoutExt;
    QString sExt;
    QString sFilePath;                                          // полный путь к файлу лога
    bool	bLogTime;                               			// признак архивирования времени
    bool	bDayly;                                             // признак ведения лога в отд.файлы для кадого дня nsme-ДД.ext
    QMutex* locker;                                             // защита

public:
    Logger(QString filename,                                    // имя файла
           bool tmdt        = true,                             // пишем дату/время каждого сообщения
           bool dayly       = false,                            // отдельный файл на каждый день в формате ИМЯ-ДД.*
           bool truncate   = false);                            // удалить существующий файл, если он есть
    ~Logger();

    void log (QString str);                                     // вывод в лог строки  с блокировкой
    QString GetActualFile();                                    // получить актуальный файл
};

#endif // LOGGER_H
