#include "responcedrives.h"

ResponceDrives::ResponceDrives()
{
    _rq = rqEmpty;
    _logger = nullptr;
}

ResponceDrives::ResponceDrives(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    QFileInfoList filesInfo = QDir::drives();
//    QDir dir;
//    _list = dir.entryList(QDir::Drives);
    for (int i=0; i<filesInfo.length(); i++)
        _list.append(filesInfo[i].absoluteFilePath());

    if (logger)
        logger->log(toString());

}

ResponceDrives::~ResponceDrives()
{

}


QByteArray ResponceDrives::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _list;

    return buf.buffer();
}

void ResponceDrives::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _list;
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

QString ResponceDrives::toString()
{
    QString msg("Список логических дисковых устройств:\r\n");
    for (int i=0; i<_list.count(); i++)
         msg += _list[i] + "\r\n";
    return msg;
}

