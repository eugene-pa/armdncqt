#include "responceread.h"

// конструктор по умолчанию для приемной стороны
ResponceRead::ResponceRead()
{
    //_rq = rqEmpty;
    _logger = nullptr;
}

// конструктор на базе запроса
ResponceRead::ResponceRead(RemoteRq& req, Logger * logger)
{
    _rq = req;
    _logger = logger;

    _srcfilepath = req.param.toString();
    _offset      = (quint64)req.param2.toULongLong();
    _length      = req.param3.toInt();
    _dstfilepath = req.param4.toString();

    QFile file(_srcfilepath);
    _exist = file.exists();
    _eof = true;
    if (_exist && file.open(QIODevice::ReadOnly))
    {
        QFileInfo info(_srcfilepath);
        _fileInfo.fill(info);
        file.seek(_offset);
        _data = file.read(_length);
        _length = _data.length();
        _eof = file.atEnd();
        file.close();
    }

    if (logger)
        logger->log(toString());
}



ResponceRead::~ResponceRead()
{

}

QByteArray ResponceRead::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _dstfilepath;
    out << _srcfilepath;
    out << _exist;
    out << _eof;
    out << _length;
    out << _offset;
    out << _fileInfo;
    out << _data;
    return buf.buffer();
}


void ResponceRead::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _dstfilepath;
            stream >> _srcfilepath;
            stream >> _exist;
            stream >> _eof;
            stream >> _length;
            stream >> _offset;
            stream >> _fileInfo;
            stream >> _data;
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


QString ResponceRead::toString()
{
    return _logger ? QString("Файл %1. Запрос блока длиной %2 начиная с позиции %3").arg(_fileInfo._name).arg(_length).arg(_offset) :
                    //QString("Файл %1. Принят блок данных длиной %2 начиная с позиции %3").arg(_fileInfo._name).arg(_length).arg(_offset);
                     //QString("Принято %1 байт начиная с позиции %2").arg(_length).arg(_offset);
                     QString("%1 / %2 байт").arg(_offset + _length).arg(_fileInfo.length());
}
