#include <QMutex>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QDate>
#include <QDebug>

#include "logger.h"

Logger::Logger(QString filename,                                    // имя файла
               bool tmdt       /*= true */,                         // пишем дату/время каждого сообщения
               bool dayly      /*= false*/,                         // отдельный файл на каждый день в формате ИМЯ-ДД.*
               bool truncate   /*= false*/)                         // удалить существующий файл, если он есть)
{
    sFilePath = filename;
    bLogTime  = tmdt;
    bDayly    = dayly;
    locker    = new QMutex();

    QFileInfo fi(sFilePath);
    if (!fi.isAbsolute())
    {
        // создаем папку лога
        QString sdir = QString("%1/%2").arg(QDir::current().absolutePath()).arg(fi.path());
        fi.dir().mkdir(sdir);

        // формируем полное имя файла
        sFilePath = QString("%1/%2").arg(sdir).arg(fi.fileName());
    }

    sFileNameWithoutExt = fi.baseName();
    sExt = fi.completeSuffix();
    dir  = fi.dir();

    if (truncate)
        QFile(GetActualFile()).remove();

}

Logger::~Logger()
{
    delete locker;
}

// актуальный файл в случае ежедневного архив формируется добавлением даты в формате: ПУТЬ/ИМЯ-ДД.*
QString Logger::GetActualFile ()
{
    return bDayly ? QString("%1/%2-%3.%4").arg(dir.absolutePath()).arg(sFileNameWithoutExt).arg(QDate::currentDate().toString("dd")).arg(sExt) : sFilePath;
}


// вывод в лог строки с блокировкой
void Logger::log (QString str)
{
    locker->lock();

    QString filename = GetActualFile();
    QFile file (filename);

    // если архив ежедневный, проверить дату обновления файла, если месячной давности - удалить
    if (bDayly && QFileInfo(file).lastModified().date().month() != QDate::currentDate().month())
    {
        file.remove();
    }

    if (file.open(QFile::Append))
    {
        QTextStream out (&file);
        QString msg = QString("%1%2").arg(bLogTime?QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss  "):"").arg(str);
        out << msg <<"\r\n";
        qDebug() << msg;                    // дублируем отладочном окне
    }

    locker->unlock();
}


