#ifndef BRIEFFILEINFO_H
#define BRIEFFILEINFO_H

#include <QFileInfo>
#include <QDateTime>
#include <QHostInfo>
#include <QBuffer>

class BriefFileInfo
{
public:
    BriefFileInfo() { }
    BriefFileInfo (QFileInfo& fi);                          // конструктор
    void fill(QFileInfo& fi);
//    void Serialize  (QDataStream& stream);
//    void Deserialize(QDataStream& stream);

    friend QDataStream &operator <<(QDataStream &stream, BriefFileInfo info)
    {
        stream << info._name;
        stream << info._lastChanged;
        stream << info._created;
        stream << info._length;
        stream << info._attrib;
        return stream;
    }
    friend QDataStream &operator >> (QDataStream &stream, BriefFileInfo info)
    {
        stream >> info._name;
        stream >> info._lastChanged;
        stream >> info._created;
        stream >> info._length;
        stream >> info._attrib;
        return stream;
    }


protected:
    QString     _name;                                      // имя файла локальное
    QDateTime   _lastChanged;                               // дата изменения
    QDateTime   _created;                                   // дата создания
    qint64      _length;                                    // длина
    QString     _attrib;                                    // атрибуты
};

#endif // BRIEFFILEINFO_H
