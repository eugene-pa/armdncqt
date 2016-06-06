#include "responcetempfile.h"

// конструктор по умолчанию для приемной стороны
ResponceTempFile::ResponceTempFile()
{

}


// конструктор на базе запроса (для передающей стороны)
ResponceTempFile::ResponceTempFile(RemoteRq& req)
{
    _rq = req;
    QString s = req.param.toString();
    _filesrc = req.param.toString();

    if (temp.open())
    {

        QFile file(_filesrc);
        _exist = file.exists();
        file.open( QIODevice::ReadOnly );
        QByteArray data = file.readAll();

        temp.write(data);
        temp.close();
        _filetemp = temp.fileName();
    }
    //file.copy();
}

ResponceTempFile::~ResponceTempFile()
{

}


QByteArray ResponceTempFile::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    // 1. Заголовок
    HeaderResponce header(_rq);
    header.Serialize(out);

    // 2. Тело ответа
    out << _filesrc;
    out << _exist;
    out << _filetemp;

    return buf.buffer();
}

void ResponceTempFile::Deserialize(QDataStream& stream)
{
    if (_rq.version <= RemoteRq::streamHeader)
    {
        if (_rq.version >= 1)
        {
            stream >> _filesrc;
            stream >> _exist;
            stream >> _filetemp;
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

QString ResponceTempFile::toString()
{
    return QString("Создан временный файл %1, копия файла %2").arg(_filetemp).arg(_filesrc);
}


