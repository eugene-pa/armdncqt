#include "responcedirs.h"

// конструктор по умолчанию для приемной стороны
ResponceDirs::ResponceDirs()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор на базе запроса
ResponceDirs::ResponceDirs(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    _folder = req.param.toString();
    QDir dir(_folder);
    _exist = dir.exists();
    _list = dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);

    if (logger)
        logger->log(toString());

}

ResponceDirs::~ResponceDirs()
{

}

QByteArray ResponceDirs::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _folder;
    out << _exist;
    out << _list;
    return buf.buffer();
}

void ResponceDirs::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _folder;
            stream >> _exist;
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


QString ResponceDirs::toString()
{
    return QString (_folder + ". Список каталогов папки:\r\n" + _list.join("\r\n"));
}
