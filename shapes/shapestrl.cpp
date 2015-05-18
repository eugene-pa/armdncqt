#include "shapestrl.h"
#include "shapeset.h"

ShapeStrl::ShapeStrl(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    QStringList lexems = src.split(' ', QString::SkipEmptyParts);
}

ShapeStrl::~ShapeStrl()
{

}

// разбор строки описания
void ShapeStrl::Parse(QString& src)
{
    Q_UNUSED(src)
}

// функция рисования
void ShapeStrl::Draw(QPainter* painter)
{
    Q_UNUSED(painter)
}

// вычисление замещаемого прямоугольника
//void ShapeStrl::GetArea(short*,short*,short*,short*)
//{

//}

bool ShapeStrl::CheckIt()
{
    return false;
}

void ShapeStrl::FixUpUnsafe()
{

}

void ShapeStrl::Prepare()
{

}

QString ShapeStrl::Dump()
{
    return "СТРЛ";
}

QString  ShapeStrl::ObjectInfo()
{
    return "СТРЛ";
}
