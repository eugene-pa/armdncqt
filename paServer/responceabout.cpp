#include <QCoreApplication>
#include <QTextCodec>

#include "responceabout.h"

// конструктор по умолчанию для приемной стороны
ResponceAbout::ResponceAbout()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор фрмирует отклик
ResponceAbout::ResponceAbout(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    QFileInfo info( QCoreApplication::applicationFilePath() );

    fileName = info.filePath();
    fileInfo.fill(info);
    hostName = QHostInfo::localHostName();
    cpu = QSysInfo::currentCpuArchitecture();
    kernel = QSysInfo::kernelType();
    osversion = QSysInfo::kernelVersion();

    QByteArray user = qgetenv("USER");
    if (user.isEmpty())
        user = qgetenv("USERNAME");
#ifdef Q_OS_WIN
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
#endif
#ifdef Q_OS_MAC
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
#ifdef Q_OS_LINUX
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
    userName = codec->toUnicode(user);

    if (logger)
        logger->log(toString());
}

// сериализация ответа
QByteArray ResponceAbout::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << fileName;
    out << fileInfo;
    out << hostName;
    out << cpu;
    out << kernel;
    out << osversion;
    out << userName;
    out << reserv3;
    out << reserv4;
    return buf.buffer();
}

ResponceAbout::~ResponceAbout()
{

}


void ResponceAbout::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> fileName;
            stream >> fileInfo;
            stream >> hostName;
            stream >> cpu;
            stream >> kernel;
            stream >> osversion;
            stream >> userName;
            stream >> reserv3;
            stream >> reserv4;
        }
        if (_rq.version >= 2)
        {

        }
    }
    else
    {
        QString msg = QString("Клиент версии %1 не поддерживает работу с сервером версии %2. Требуется обновление ПО клиента").arg(RemoteRq::streamHeader).arg(_rq.version);
        log(msg);
    }
}

QString ResponceAbout::toString()
{
    return QString("CPU: %1\r\nОС: %2\r\nVersion: %3\r\nХост: %4\r\nUSER: %5\r\nFile: %6\r\n").arg(cpu).arg(kernel).arg(osversion).arg(hostName).arg(userName).arg(fileName);
}
