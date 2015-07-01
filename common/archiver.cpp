#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "archiver.h"


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

}

// конструктор ридера
ArhReader::ArhReader(QString& filename)
{
    this->filename = filename;
    dir = QFileInfo(filename).absoluteDir();
    ext = QFileInfo(filename).suffix();
    QRegularExpressionMatch match = QRegularExpression(".+\\D+(?=\\d+\\..+)").match(filename);
    if (match.hasMatch())
        prefix = match.captured();
    file.setFileName(filename);
    file.open(QIODevice::ReadOnly);
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
    return length == Length() ? length : -1;
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
    file.seek(file.pos() - 4);
    int pos;
    file.read((char *)&pos, 4);
    file.seek(pos);
    return Next();
}