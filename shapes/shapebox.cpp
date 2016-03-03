#include "shapeset.h"
#include "shapebox.h"

QPen ShapeBox::pen;                                        // перо

ShapeBox::ShapeBox(QString& src, ShapeSet* parent) : DShape(src, parent)
{
    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeRc. Исключение при разборе строки: %1").arg(src));
    }

}

ShapeBox::~ShapeBox()
{

}

void ShapeBox::InitInstruments()
{
    pen = QPen(colorScheme->GetColor("Box").lighter(), 1);
}

// 0   1   2     3     4     5
//     Ст  X1    Y1    X2    Y2
// 4   2   753   281   763   283
void ShapeBox::Parse(QString& src)
{
    bool ok  = true,
         ret = false;
    QStringList lexems = src.split(' ',QString::SkipEmptyParts);        // лексемы
    // 0- примитив
    type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
    // 1- номер станции
    idst = lexems[1].toInt(&ret);               ok &= ret;
    st      = Station::GetById(idst);                   // станция

    // 2-5- координаты
    x1    = lexems[2].toFloat(&ret);            ok &= ret;
    y1    = lexems[3].toFloat(&ret);            ok &= ret;
    x2    = lexems[4].toFloat(&ret);            ok &= ret;
    y2    = lexems[5].toFloat(&ret);            ok &= ret;

    setDimensions ();
}

void ShapeBox::accept()
{

}

void ShapeBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw (painter);
}

void ShapeBox::Draw (QPainter* painter)
{
    accept();

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect);
}
