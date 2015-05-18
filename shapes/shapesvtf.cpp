#include "shapesvtf.h"

ShapeSvtf::ShapeSvtf(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    QStringList lexems = src.split(' ', QString::SkipEmptyParts);
}

ShapeSvtf::~ShapeSvtf()
{

}

// разбор строки описания
void ShapeSvtf::Parse(QString& src)
{
    Q_UNUSED(src)
}

// функция рисования
void ShapeSvtf::Draw(QPainter* painter)
{
    Q_UNUSED(painter)
}

// вычисление замещаемого прямоугольника
//void ShapeSvtf::GetArea(short*,short*,short*,short*)
//{

//}

bool ShapeSvtf::CheckIt()
{
    return false;
}

void ShapeSvtf::FixUpUnsafe()
{

}

void ShapeSvtf::Prepare()
{

}

QString ShapeSvtf::Dump()
{
    return "СВТФ";
}

QString  ShapeSvtf::ObjectInfo()
{
    return "СВТФ";
}
