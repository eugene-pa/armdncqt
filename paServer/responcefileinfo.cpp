#include "responcefileinfo.h"

// конструктор по умолчанию для приемной стороны
ResponceFileInfo::ResponceFileInfo()
{

}

// конструктор на базе запроса
ResponceFileInfo::ResponceFileInfo(RemoteRq& req)
{
    _rq = req;
    _filepath = req.param.toString();

    QFileInfo info(_filepath);
    _fileInfo.fill(info);
    _exist = info.exists();
}

ResponceFileInfo::~ResponceFileInfo()
{

}

QByteArray ResponceFileInfo::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _exist;
    out << _fileInfo;

    return buf.buffer();
}

void ResponceFileInfo::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _exist;
            stream >> _fileInfo;
        }
    }
}


QString ResponceFileInfo::toString()
{
    QString name = _fileInfo._name;
    return QString("Файл: %1\r\nРазмер: %2\r\nСоздан: %3\r\nИзменен: %4\r\nАтрибуты: %5\r\n").arg(_fileInfo._name).arg(_fileInfo._length).arg(_fileInfo._created.toString(FORMAT_DATETIME)).arg(_fileInfo._lastChanged.toString(FORMAT_DATETIME)).arg(_fileInfo._attrib);
}
