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
        return file->read((char*)this, sizeof(ArchiveHeader)-sizeof(offset));
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
int ArhReader::Read()
{
    return Header.Read(&file);
}

