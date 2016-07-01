#ifndef LOGGER_H
#define LOGGER_H

#include <QDir>
#include <QMutex>
#include "defines.h"

class Logger
{
public:
    Logger(QString filename,                                    // имя файла
           bool tmdt        = true,                             // пишем дату/время каждого сообщения
           bool dayly       = false,                            // отдельный файл на каждый день в формате ИМЯ-ДД.*
           bool truncate   = false);                            // удалить существующий файл, если он есть
    ~Logger();

    QString log (QString str);                                  // вывод в лог строки  с блокировкой
    QString GetActualFile();                                    // получить актуальный файл

    // статические открытые функции
    static void SetLoger(Logger * p) { logger = p; }            // установить активный статический логгер
    static void LogStr (QString str, LogSourcer = SRC_UNDEFINED, LogTypes = TYP_UNDEFINED); // протоколирование строки в SQL и логе; если сохраняем в SQL, отображаем в строке сообщений
    static QString GetHex(char * data, int maxlength=16);
    static QString GetHex(QByteArray& array, int maxlength=16);

private:
    static  Logger * logger;                                    // статический экземпляр логгера
    QDir    dir;                                                // каталог
    QString sFileNameWithoutExt;
    QString sExt;
    QString sFilePath;                                          // полный путь к файлу лога
    bool	bLogTime;                               			// признак архивирования времени
    bool	bDayly;                                             // признак ведения лога в отд.файлы для кадого дня nsme-ДД.ext
    QMutex* locker;                                             // защита
};

#endif // LOGGER_H
