#include "shaperc.h"
#include "shapeset.h"
#include "colorscheme.h"
#include "../spr/station.h"

QPen *ShapeRc::PenFree;                                     // свободная РЦ
QPen *ShapeRc::PenBusy;                                     // занятая РЦ (если занятая РЦ замкнута - контур замыкания вокруг)
QPen *ShapeRc::PenRqRoute;                                  // в устанавливаемом маршруте
QPen *ShapeRc::PenPzdRoute;                                 // в поездном маршруте
QPen *ShapeRc::PenMnvRoute;                                 // в маневровом маршруте
QPen *ShapeRc::PenZmk;                                      // замкнутая РЦ не в неиспользованном маршруте
QPen *ShapeRc::PenZmkContur;                                // замкнутая РЦ для контура (рисуется поверх незаполненным контуром)
QPen *ShapeRc::PenZmkConturMnv;                             // замкнутая РЦ для контура в маневровом маршруте(рисуется поверх незаполненным контуром)
QPen *ShapeRc::PenIr;                                       // искусственная разделка (мигает поверх других состояний)
QPen *ShapeRc::PenExpired;                                  // ТС устарели
QPen *ShapeRc::PenUndefined;                                // объект неопределен - пассивная отрисовка


ShapeRc::ShapeRc(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    sprRc = nullptr;
    combined = false;

    Parse(src);
    normalize();
    setDimensions ();
}

ShapeRc::~ShapeRc()
{

}

// инициализация статических инструментов отрисовки
void ShapeRc::InitInstruments()
{
    PenFree     = new QPen (QBrush(colorScheme->GetColor("Normal"   )), mThick);    // свободная РЦ
    PenBusy     = new QPen (QBrush(colorScheme->GetColor("Busy"     )), mThick);    // занятая РЦ (если занятая РЦ замкнута - контур замыкания вокруг)
    PenRqRoute  = new QPen (QBrush(colorScheme->GetColor("RouteSet" )), mThick);    // в устанавливаемом маршруте
    PenPzdRoute = new QPen (QBrush(colorScheme->GetColor("RoutePzd" )), mThick);    // в поездном маршруте
    PenMnvRoute = new QPen (QBrush(colorScheme->GetColor("RouteMnv" )), mThick);    // в маневровом маршруте
    PenZmk      = new QPen (QBrush(colorScheme->GetColor("Zmk"      )), mThick);    // замкнутая РЦ не в неиспользованном маршруте
    PenZmkContur= new QPen (QBrush(colorScheme->GetColor("RoutePzd" )), mThick);    // замкнутая РЦ для контура (рисуется поверх незаполненным контуром)
    PenZmkConturMnv= new QPen (QBrush(colorScheme->GetColor("RouteMnv")),mThick);   // замкнутая РЦ для контура в маневровом маршруте(рисуется поверх незаполненным контуром)
    PenIr       = new QPen (QBrush(colorScheme->GetColor("Ir"       )), mThick);    // искусственная разделка (мигает поверх других состояний)
    PenExpired  = new QPen (QBrush(colorScheme->GetColor("Expired"  )), mThick);    // ТС устарели
    PenUndefined= new QPen (QBrush(colorScheme->GetColor("Undefined")), mThick);    // объект неопределен - пассивная отрисовка
}



// разбор строки описания
// 0     1    2    3    4       5    6      7     8      9      10
//      Ст   Тип   X1   Y1     X2    Y2     РЦ   Стр1   Стр2   Стр3
// 16    2    1   1174  522   1233  522     45     0      0      0
void ShapeRc::Parse(QString& src)
{
    bool ok  = true,
         ret = false;
    QStringList lexems = src.split(' ',QString::SkipEmptyParts);
    if (lexems.length() == 11)
    {
        type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
        idst = lexems[1].toInt(&ret);               ok &= ret;
        subtype = lexems[2].toInt(&ret);            ok &= ret;

        if (type == ANGLE_COD)
        {
            switch (subtype)
            {
                case 0: subtype = SWTONE;   break;
                case 1: subtype = NWTOSE;   break;
                default:subtype = Free;     break;
            }
        }
        x1    = lexems[3].toFloat(&ret);            ok &= ret;
        y1    = lexems[4].toFloat(&ret);            ok &= ret;
        x2    = lexems[5].toFloat(&ret);            ok &= ret;
        y2    = lexems[6].toFloat(&ret);            ok &= ret;
        setDimensions ();

        idObj = lexems[7].toInt(&ret);              ok &= ret;

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        sprRc   = Rc::GetById(idObj);                       // РЦ

        for (int i=8; i<11; i++)
        {
            if (int nostrl = lexems[7].toInt(&ret) && ret)
                strl.append(new LinkedStrl(nostrl));
            ok &= ret;
        }

        if (!ok)
        {
            log (QString("%1: %2").arg("Ошибка синтаксиса примитива").arg(src));
        }
    }
    else
    {
        log (QString("%1: %2").arg("Ошибка синтаксиса примитива (ош.числа лексем)").arg(src));
    }
}

// пока нет отрисовки дуг, заменяем дуги отрезками
void ShapeRc::normalize()
{
    float R = (x2 - x1) / 2;
    switch (subtype)
    {
        case ArcNW: y2 = y1;    y1 = y1-R;  x2 = x1-R;      break;
        case ArcNE: x2 = x1;    x1 += R;    y2 = y1 - R;    break;
        case ArcSE: x2 = 1 + R; y2 = y1;    y1 += R;        break;
        case ArcSW: x2 = x1;    x1 -= R;    y2 = y1 + R;    break;
        case ArcSWN:y2 = y1 - R;y1 += R;    x2 = x1;        break;
        case ArcNES:y2 = y1 - R;y1 += R;    x2 = x1;        break;

        case SWTONE:                                        // Учтем ориентацию наклонного отрезка
            float x = x2; x2 = x1; x1 = x; break;
    }
}

// функция рисования
void ShapeRc::Draw(QPainter* painter)
{
    //QBrush brush(Qt::black);
    QColor color(Qt::black);

    //if (sprRc != nullptr)

    QPen * pen = sprRc == nullptr ? PenUndefined : sprRc->StsBusy() ? PenBusy : sprRc->StsZmk() ? PenZmk : PenFree;
    //pen.setCosmetic(true);
    pen->setCapStyle(Qt::FlatCap);
    if(y1 != y2)
        painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(*pen);
    painter->drawLine(rect.topLeft(), rect.bottomRight());
    //setScale(2);
}
void ShapeRc::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}

//// вычисление замещаемого прямоугольника
//void ShapeRc::GetArea(short*,short*,short*,short*)
//{

//}

bool ShapeRc::CheckIt()
{
    return false;
}

void ShapeRc::FixUpUnsafe()
{

}

void ShapeRc::Prepare()
{

}

QString ShapeRc::Dump()
{
    return "РЦ";
}

QString  ShapeRc::ObjectInfo()
{
    return "РЦ";
}
