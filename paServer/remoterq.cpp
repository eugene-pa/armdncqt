#include "remoterq.h"

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

Responce::Responce()
{

}
Responce::~Responce()
{

}
