#ifndef INIREADER_H
#define INIREADER_H

#include "defines.h"

class IniReader
{
public:
    IniReader(QString inifile);
    ~IniReader();

private:
    bool readIniFile (QString& spath, bool bUnicode=true);  // чтение опций из файла
    QFile file;
    QString inifile;
    QHash <QString*,QString*> options;                      // опции, индексированные по ключу-имени опции

};

#endif // INIREADER_H
