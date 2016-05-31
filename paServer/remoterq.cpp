#include "remoterq.h"

const quint32 RemoteRq::streamHeader = 0x55aa55aa;                 // заголовок
const quint16 RemoteRq::paServerVersion = 1;                       // версия paServer


RemoteRq::RemoteRq()
{
    rq = rqEmpty;
    remotePath = "";
    reserv1 = "";
    reserv2 = 0;
}

RemoteRq::~RemoteRq()
{

}

// сформировать пустой ответ
// виртуальная функция должна быть переопределена в наследуемых классах
QByteArray RemoteRq::prepare()
{
    return QByteArray();
}

// конструктор
BriefFileInfo::BriefFileInfo (QFileInfo& fi)
{
    fill(fi);
}

void BriefFileInfo::fill(QFileInfo& fi)
{
    _name           =  fi.fileName();
    _lastChanged    = fi.lastModified();
    _created        = fi.created();
    _length         = fi.size();

    _attrib         = "";
    if (!fi.isWritable()) _attrib += "R";
    if (fi.isHidden())    _attrib += "H";
    if (!fi.isExecutable()) _attrib += "B";
}

/*
void BriefFileInfo::Serialize(QDataStream& stream)
{
    stream << _name;                                        // имя файла локальное
    stream << _lastChanged;                                 // дата изменения
    stream << _created;                                     // дата создания
    stream << _length;                                      // длина
    stream << _attrib;                                      // атрибуты
}

void BriefFileInfo::Deserialize(QDataStream& stream)
{
    stream >> _name;                                        // имя файла локальное
    stream >> _lastChanged;                                 // дата изменения
    stream >> _created;                                     // дата создания
    stream >> _length;                                      // длина
    stream >> _attrib;                                      // атрибуты
}
*/
