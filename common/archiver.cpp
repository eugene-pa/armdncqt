#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtMath>
#include "archiver.h"


// конструктор по умолчанию
ArchiveHeader::ArchiveHeader(int l, WORD t, WORD r)
{
    signature = SIGNATURE;
    Rsrv.w = r;                                                 // резерв - доп.инфо
    time = (quint32)QDateTime::currentDateTime().toTime_t();
    type = t;                                                   // тип
    length = (WORD)l;                                           // длина данных
}

// конструктор на основе открытого и позиционированного файла
ArchiveHeader::ArchiveHeader(QFile * file)
{
    Read(file);
}

int ArchiveHeader::Read(QFile * file)
{
    if (file != nullptr)
    {
        offset = file->pos();
        return file->read((char*)this, headersize);
    }
    else
    {
        qDebug() << "Конструктору ArchiveHeader передан нулевой указатель";
    }
    return -1;
}


ArhReader::~ArhReader()
{
    qDebug() << "~ArhReader()";
}

// конструктор ридера
ArhReader::ArhReader(QString filename)
{
    dir = QFileInfo(filename).absoluteDir();
    ext = QFileInfo(filename).suffix();
    QRegularExpressionMatch match = QRegularExpression(".+\\D+(?=\\d+\\..+)").match(filename);
    if (match.hasMatch())
        prefix = match.captured();
    this->filename = filename;
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly);
}

// конструктор для чтения
ArhReader::ArhReader(QString dir, QString prefix)
{
    this->dir = dir;
    this->prefix = prefix;
    ext = "arh";
}

// чтение записи от текущего положения указателя
int ArhReader::Next()
{
    int length = 0;
    if (header.Read(&file) == ArchiveHeader::headersize)
    {
        length = file.read(data, Length() + 4);             // считываем вместе с оконечным укзателем на запись
        if (header.offset != *((int *)(data+length-4)))     // проверяем соответствие смещения записи указателю на начало записи за данными
            qDebug() << "Нарушение формата архива";
    }
    if (qAbs((long)_rqt.toTime_t() - (long)time()) > 3600)
        return -1;
    return length - 4 == Length() ? length : -1;
}

// чтение записи по дате >= заданной
int ArhReader::Read (QDateTime t)
{
    if (t.toTime_t() < time())
    {
        do
        {
            if (Prev() < 0)
                return -1;
            // если разница более 1 часа - не найдем
            if (qAbs((long)t.toTime_t() - (long)time()) > 3600)
                return -1;
        }
        while (header.time > t.toTime_t());
    }
    else
    {
        do
        {
            if (Next() < 0)
                return -1;
            // если разница более 1 часа - не найдем
            if (qAbs((long)t.toTime_t() - (long)time()) > 3600)
                return -1;
        }
        while (header.time < t.toTime_t());
    }
    return Length();
}

// чтение записи от текущего положения указателя
int ArhReader::First()
{
    file.seek(0);
    return Next();
}

// чтение записи от текущего положения указателя
int ArhReader::Last()
{
    file.seek(file.size());
    return Prev();
}

// чтение записи от текущего положения указателя
int ArhReader::Prev()
{
    int pos = file.pos();
    if (pos==0)
        return -1;
    file.seek(pos - 4);
    file.read((char *)&pos, 4);
    file.seek(pos);
    int ret = Next();
    file.seek(pos);
    return ret == -1 ? ret : Length();
}

// получить архивное имя по дате и времени
bool ArhReader::setArhName(QDateTime t)
{
    _rqt = t;
    filename = QString("%1%2.arh").arg(prefix).arg(t.time().hour());
    file.setFileName(dir.path() + "/" + filename);
    return file.open(QIODevice::ReadOnly);
}

// получить имя суточного ZIP-архива по дате
QString ArhReader::getZipName(QDateTime t)
{
    _rqt = t;
    return QString("%1%2%3.zip").arg(t.date().year()).arg(t.date().month(),2,10,QChar('0')).arg(t.date().day(),2,10,QChar('0'));
}

// перейти на начало следующего часа и установить новое имя часового архива по дате
bool ArhReader::setNextHour(QDateTime t)
{
    int dt = (60 - t.time().minute())*60 + 60 - t.time().second();
    return setArhName(QDateTime::fromTime_t(t.toTime_t() + dt));
}

// перейти на начало предыдущего часа и установить новое имя часового архива по дате
bool ArhReader::setPrevHour(QDateTime t)
{
    int dt = t.time().minute()*60 + t.time().second() + 1;
    return setArhName(QDateTime::fromTime_t(t.toTime_t() - dt));
}

//======================================================================================================================================

// конструктор для записи
ArhWriter::ArhWriter(QString s, QString prefix)
{
    dir = s;    // QFileInfo(filename).absoluteDir();
    QFileInfo fi(dir + "/test");
    if (fi.isRelative())
    {
        // создаем папку лога
        dir = QString("%1/%2").arg(QDir::current().absolutePath()).arg(fi.path());
        fi.dir().mkdir(dir);
    }

    this->prefix = prefix;
}

// запись данных в архив
void ArhWriter::Save(void *data, int length, WORD type, WORD r)
{
    ArchiveHeader header(length, type, r);
    QString name = QString("%1/%2%3.arh").arg(dir).arg(prefix).arg(QTime::currentTime().hour());
    QFile file (name);
    file.open(QIODevice::Append);
    qint32 pos = (quint32)file.pos();
    file.write((const char*)(&header), ArchiveHeader::headersize);
    file.write((const char*)data, length);
    file.write((const char*)&pos,4);
}


