#include "responceabout.h"

// конструктор фрмирует отклик
ResponceAbout::ResponceAbout()
{
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
}

QByteArray ResponceAbout::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);

    QDataStream out(&buf);
    out << RemoteRq::streamHeader;
    out << RemoteRq::paServerVersion;

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


void ResponceAbout::Deserialize(QByteArray& data)
{
    quint32 header;                                          // заголовок
    quint16 version;                                         // версия paServer

    QBuffer buf(&data, nullptr);
    buf.open(QIODevice::ReadOnly);

    QDataStream stream(&buf);
    stream >> header;
    stream >> version;
    if (header == RemoteRq::streamHeader)
    {
        if (version <= RemoteRq::streamHeader)
        {
            if (version >= 1)
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
            if (version >= 2)
            {

            }
        }
        else
        {
            QString msg = QString("Клиент версии {1} не поддерживает работу с сервером версии {2}. Требуется обновление ПО клиента").arg(RemoteRq::streamHeader).arg(version);
            log(msg);
        }
    }
    else
    {
        QString msg = QString("Нет сигнатуры сериализации в потоке данных");
        log(msg);
    }

}

