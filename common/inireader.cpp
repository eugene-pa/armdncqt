#include "inireader.h"

IniReader::IniReader(QString inifile)
{
    this->inifile = inifile;
    readIniFile (this->inifile = inifile, true);            // чтение опций из файла
}

IniReader::~IniReader()
{

}


// чтение опций из файла
bool IniReader::readIniFile (QString& spath, bool bUnicode)
{
    bool ret = true;
    file.setFileName(spath);
    file.open(QIODevice::ReadOnly);
    while (true)
    {
        QByteArray line = file.readLine(255);
        if (line.length()==0)
            break;
    }
    return ret;
}
