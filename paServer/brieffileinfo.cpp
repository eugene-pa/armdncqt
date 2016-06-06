#include "brieffileinfo.h"

// конструктор
BriefFileInfo::BriefFileInfo (QFileInfo& fi)
{
    fill(fi);
}

void BriefFileInfo::fill(QFileInfo& fi)
{
    bool _exist = fi.exists();
    _name           =  fi.fileName();
    _lastChanged    = fi.lastModified();
    _created        = fi.created();
    _length         = fi.size();

    _attrib         = "";
    if (fi.isReadable()) _attrib += "R";
    if (fi.isWritable()) _attrib += "W";
    if (fi.isHidden())    _attrib += "H";
    if (fi.isExecutable()) _attrib += "E";
}

