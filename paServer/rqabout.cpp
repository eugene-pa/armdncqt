#include "rqabout.h"
#include <QSysInfo>

RqAbout::RqAbout()
{
    rq = rqAbout;
}

RqAbout::~RqAbout()
{

}

QByteArray RqAbout::Serialize()
{
    return QByteArray((const char*)this, sizeof(QByteArray));
}


ResponceAbout::ResponceAbout()
{
    QFileInfo info( QCoreApplication::applicationFilePath() );

    fileName = info.filePath();
    fileInfo.fill(info);
    hostName = QHostInfo::localHostName();
    cpu = QSysInfo::currentCpuArchitecture();
    kernel = QSysInfo::kernelType();
    version = QSysInfo::kernelVersion();

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
    return QByteArray((const char*)this, sizeof(QByteArray));
}
