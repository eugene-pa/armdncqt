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
    //QString root = QDir::currentPath();
    bool ret = true;
    QFile file (spath);
    if (file.open(QFile::ReadOnly))
    {
        QTextStream in (&file);
        in.setCodec(bUnicode ? "UTF-8" : "Windows-1251");
        QString str;

        while (!(str = in.readLine().trimmed()).isNull())   // читаем построчно до упора
        {
            if (str[0]==';' || str[0]=='/')                 // отсекаем комментарии
                continue;
            if (QRegularExpression("\\A[^;/=]*=").match(str).hasMatch())
            {
                QString name  = QRegularExpression("[^=^ ]+").match(str).captured();
                QString value = QRegularExpression("(?<=[^;^/]=)[^ ]+").match(str).captured();
                options.insert(name, value);
            }
        }

    }
    return ret;
}

// получить текстовое значение i-ой опции option
bool IniReader::GetText(QString option, QString& value, int indx)
{
    value = "";
    QMultiMap<QString,QString> ::iterator i = options.find(option);
    int j = 0;
    //while (i != options.end() && i.key() == option)
    for (; i != options.end() && i.key() == option; i++)
    {
        if ( j++ == indx)
        {
            value = i.value();
            return true;
        }
    }
    return false;
}

// получить целое значение i-ой опции option
bool IniReader::GetInt (QString option, int& value, int indx)
{
    value = 0;
    bool ret = false;
    QString s;
    if (GetText(option, s, indx))
        value = s.toInt(&ret);
    return ret;
}

// получить догическое значение i-ой опции option
bool IniReader::GetBool(QString option, bool   & value, int indx)
{
    value = 0;
    bool ret = false;
    QString s;
    if (GetText(option, s, indx))
    {
        s = s.toUpper();
        value = s=="ON" || s=="TRUE";
    }
    return ret;
}
