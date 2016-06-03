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

