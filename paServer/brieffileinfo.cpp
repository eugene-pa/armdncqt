#include "brieffileinfo.h"

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

    // танцы с бубном: для ярлыков функция QFileInfo::size возыращает размер ссылочного файла, а не ярлыка
    // приходится читать ярлык и определять его размер по размеру считанных данных
    if (fi.isSymLink())
    {
        QFile f(fi.filePath());
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray data = f.readAll();
            _length = data.length();
        }
    }

    _attrib         = "";
    if (fi.isReadable()) _attrib += "R";
    if (fi.isWritable()) _attrib += "W";
    if (fi.isHidden())    _attrib += "H";
    if (fi.isExecutable()) _attrib += "E";
}

