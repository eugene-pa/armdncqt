#include <QMutex>
#include <QFileInfo>
#include <QTextStream>
#include <QDir>
#include <QDate>
#include <QDebug>
#include <QTextCodec>

#include "logger.h"

Logger * Logger::logger;                                            // статический экземпляр логгера

Logger::Logger(QString filename,                                    // имя файла
               bool tmdt       /*= true */,                         // пишем дату/время каждого сообщения
               bool dayly      /*= false*/,                         // отдельный файл на каждый день в формате ИМЯ-ДД.*
               bool truncate   /*= false*/)                         // удалить существующий файл, если он есть)
{
    bLogTime  = tmdt;
    bDayly    = dayly;
    locker    = new QMutex();
    logger    = nullptr;
    this->truncate  = truncate;

    init(filename);
}

Logger::~Logger()
{
    delete locker;
}

void Logger::init(QString& filename)
{
    sFilePath = filename;
    QFileInfo fi(sFilePath);
    if (fi.isRelative())
    {
        // создаем папку лога
        QString sdir = QString("%1/%2").arg(QDir::current().absolutePath()).arg(fi.path());
        fi.dir().mkdir(sdir);

        // формируем полное имя файла
        sFilePath = QString("%1/%2").arg(sdir).arg(fi.fileName());
    }
    else
    {
        // созданм папку по абсолютному пути
        if (!fi.dir().exists())
            QDir().mkdir(fi.dir().path());
    }
    sFileNameWithoutExt = fi.baseName();
    sExt = fi.completeSuffix();
    dir  = fi.dir();

    if (truncate)
        QFile(GetActualFile()).remove();
}

// актуальный файл в случае ежедневного архив формируется добавлением даты в формате: ПУТЬ/ИМЯ-ДД.*
QString Logger::GetActualFile ()
{
    return bDayly ? QString("%1/%2-%3.%4").arg(dir.absolutePath()).arg(sFileNameWithoutExt).arg(QDate::currentDate().toString("dd")).arg(sExt) : sFilePath;
}

// изменение файла лога и его размещения
void Logger::ChangeActualFile(QString filename)
{
    init(filename);
}


// вывод в лог строки с блокировкой
QString Logger::log (QString str)
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
#ifdef Q_OS_WIN
        QTextCodec::setCodecForLocale( QTextCodec::codecForName("Windows-1251"));
#endif
        QTextStream out (&file);
        QString msg = QString("%1%2").arg(bLogTime?QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss  "):"").arg(str);
        out << msg <<"\r\n";
#ifdef Q_OS_WIN
        QTextCodec::setCodecForLocale( QTextCodec::codecForName("CP866"));
#endif
        qDebug() << msg.toStdString().c_str();                    // дублируем отладочном окне
    }

    locker->unlock();
    return str;
}


// статическая функция протоколирования строки в SQL и логе; если сохраняем в SQL, отображаем в строке сообщений
// странно, но такое решение приводит к искажению кодировки входной строки при вызове функции из другого потока
void Logger::LogStr (QString str, LogSourcer source, LogTypes type)
{
    if (source!=SRC_UNDEFINED && type!=TYP_UNDEFINED)
    {

    }
    if (logger != nullptr)
        logger->log(str);
    else
        qDebug() << qPrintable(str);
}

// получить 16-ричную строку
QString Logger::GetHex(char * data, int length)
{
    QString tmp;
    for (int i=0; i<length; i++)
    {
        tmp += QString("%1 ").arg((BYTE)data[i],2,16,QChar('0')).toUpper();
    }
    return tmp;
}

QString Logger::GetHex(QByteArray& array, int maxlength)
{
    QString tmp;
    for (int i=0; i<array.length() && i<maxlength; i++)
    {
        tmp += QString("%1 ").arg((BYTE)array[i],2,16,QChar('0')).toUpper();
    }
    return tmp;
}
