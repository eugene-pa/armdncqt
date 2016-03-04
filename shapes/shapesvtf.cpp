#include "shapeset.h"
#include "shapesvtf.h"

int ShapeSvtf::diametr = 15;                                // диаметр
int ShapeSvtf::baseW  ;                                     // ширина основания
int ShapeSvtf::standH ;                                     // высота ножки

// статические ресурсы для отрисовки
QPen * ShapeSvtf::MainPen;                                  // перо основное
QPen * ShapeSvtf::MainPen2;                                 // перо двойной линии
QPen * ShapeSvtf::PenUndefined;                             // неопред.состояние
QPen * ShapeSvtf::PenExpired;                               // устарели ТС
QPen * ShapeSvtf::PenAlarm;                                 // авария светофора
QPen * ShapeSvtf::PenText;                                  // надпись

QBrush * ShapeSvtf::BrushPzdOn;                             // поездной открыт
QBrush * ShapeSvtf::BrushYelllow;                           // желтый
QBrush * ShapeSvtf::BrushPzdOff;                            // поездной закрыт
QBrush * ShapeSvtf::BrushPzdInOff;                          // поездной входной закрыт
QBrush * ShapeSvtf::BrushMnvOn;                             // маневровый открыт
QBrush * ShapeSvtf::BrushMnvOff;                            // маневровый закрыт
QBrush * ShapeSvtf::BrushUndefined;                         // неопред.состояние
QBrush * ShapeSvtf::BrushExpired;                           // устарели ТС
QBrush * ShapeSvtf::BrushLockBackground;                    // фон при блокировке
QBrush * ShapeSvtf::BrushAdBackground;                      // фон при автодействии
QBrush * ShapeSvtf::BrushOmBackground;                      // фон при отмене маршрута
QBrush * ShapeSvtf::BrushAlarmBackground;                   // фон при аварии

QFont * ShapeSvtf::font;                                    // шрифт отрисовки названия

#ifdef Q_OS_WIN
int ShapeSvtf::fontsize = 10;                               // размер шрифта
int ShapeSvtf::offset_x = -2,                               // смещение текста
    ShapeSvtf::offset_y = -2;
QString ShapeSvtf::fontname = "Segoe UI";                   // шрифт
#endif
#ifdef Q_OS_MAC
int ShapeSvtf::fontsize = 14;                               // размер шрифта
int ShapeSvtf::offset_x = -2,                               // смещение текста
    ShapeSvtf::offset_y = 0;
QString ShapeSvtf::fontname = "Segoe UI";                   // шрифт
#endif
#ifdef Q_OS_LINUX
int ShapeSvtf::fontsize = 11;                               // размер шрифта
int ShapeSvtf::offset_x = -2,                               // смещение текста
    ShapeSvtf::offset_y = 0;
QString ShapeSvtf::fontname = "Segoe UI";                   // шрифт
#endif

ShapeSvtf::ShapeSvtf(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    svtf  = nullptr;                                        // светофор поездной
    svtfM = nullptr;                                        // светофор совмещенный
    prop  = nullptr;                                        // описатель геометрии
    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeSvtf. Исключение при разборе строки: %1").arg(src));
    }

}

ShapeSvtf::~ShapeSvtf()
{

}


void ShapeSvtf::InitInstruments()
{
    BrushPzdOn         = new QBrush(colorScheme->GetColor("SvtfPzdOpen" ));       // поездной открыт
    BrushYelllow       = new QBrush(colorScheme->GetColor("SvtfYellow"  ));       // желтый
    BrushPzdOff        = new QBrush(colorScheme->GetColor("SvtfPzdClose"));       // поездной закрыт
    BrushPzdInOff      = new QBrush(colorScheme->GetColor("SvtfInClose" ));       // поездной входной закрыт
    BrushMnvOn         = new QBrush(colorScheme->GetColor("SvtfMnvOpen" ));       // маневровый открыт
    BrushMnvOff        = new QBrush(colorScheme->GetColor("SvtfMnvClose"));       // маневровый закрыт
    BrushUndefined     = new QBrush(colorScheme->GetColor("Undefined"   ));       // неопред.состояние
    BrushExpired       = new QBrush(colorScheme->GetColor("Expired"     ));       // устарели ТС
    BrushLockBackground= new QBrush(colorScheme->GetColor("SvtfLockBackground")); // фон при блокировке
    BrushAdBackground  = new QBrush(colorScheme->GetColor("SvtfAdBackground"  )); // фон при автодействии
    BrushOmBackground  = new QBrush(colorScheme->GetColor("SvtfOmBackground"  )); // фон при отмене маршрута
    BrushAlarmBackground= new QBrush(Qt::red);                                    // фон при аварии colorScheme->GetColor("SvtfAlarmBackground"

    PenUndefined       = new QPen (*BrushUndefined, 1);                                 // одинарная линия отрисовки неопред. состояния светофора
    PenExpired         = new QPen (*BrushExpired, 1);                                   // одинарная линия отрисовки состояния светофора при устаревании ТС
    MainPen            = new QPen (QBrush(colorScheme->GetColor("SvtfLinesNormal")), 1);// одинарная линия отрисовки светофора
    MainPen2           = new QPen (QBrush(colorScheme->GetColor("SvtfLinesNormal")), 2);// двойная линия отрисовки светофора
    PenAlarm           = new QPen (QBrush(colorScheme->GetColor("SvtfAlarmCross")), 1);// авария светофора
    PenText            = new QPen (Qt::darkBlue,1);                                 // надпись

    font = new QFont(fontname,fontsize);
}

// разбор строки описания
//      СТ ТИП  X1  Y1   X2   Y2    No  NoExt Имя
//  0   1   2   3    4    5    6    7    8     9
// 18   2   4  1058 447   0    0    68   69   $Ч3
void ShapeSvtf::Parse(QString& src)
{
    bool ok  = true,
         ret = false;

    QStringList lexems = src.split(' ', QString::SkipEmptyParts);
    if (lexems.length() >= 10)
    {
        // 0 - примитв
        type    = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;

        // 1 - станция
        idst    = lexems[1].toInt(&ret);               ok &= ret;

        // 2 - тип стрелки
        subtype = lexems[2].toInt(&ret);               ok &= ret;

        // 3-6 - координаты
        x1    = lexems[3].toFloat(&ret);               ok &= ret;
        y1    = lexems[4].toFloat(&ret);               ok &= ret;
        x2    = lexems[5].toFloat(&ret);               ok &= ret;
        y2    = lexems[6].toFloat(&ret);               ok &= ret;

        if (x2 == 0)
            x2 = x1 + 23;
        if (y2 == 0)
            y2 = y1 + 20;
        setDimensions ();

        // к примитиву могут быть привязаны 2 светофора (1 и 2)
        // по-хорошему, первый должен быть поездной, второй маневровый
        // боюсь, что могут быть кадры, где маневровый указан в первом поле (в поле поездного)
        // хотелось бы разрулить эту ситуацию
        // 7,8. Первый и второй светофоры
        int idSvtf1 = lexems[7].toInt(&ret);
        int idSvtf2 = lexems[8].toInt(&ret);
        idObj = idSvtf1 ? idSvtf1 : idSvtf2;                // избыточность

        // 9 - имя
        name = lexems[9].replace("$","");                   // имя светофора

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        // поездной и маневровый светофоры
        Svtf * p;
        if (idSvtf1 && (p=Svtf::GetById(idSvtf1)))          // 1 светофор
        {
            if (!p->IsTypeMnv())    svtf  = p;
            else                    svtfM = p;
        }
        if (idSvtf2 && (p=Svtf::GetById(idSvtf2)))          // 2 светофор
        {
            if (!p->IsTypeMnv())    svtf  = p;
            else                    svtfM = p;
        }

        // указатель на свойства (проверить допустимость!)
        prop = &svtfProp[subtype];

        // геометрия отрисовки основания и ножки
        base = QLineF(XY + QPointF(prop->bx1, prop->by1), XY + QPointF(prop->bx2, prop->by2));// основание
        stand= QLineF(XY + QPointF(prop->hx1, prop->hy1), XY + QPointF(prop->hx2, prop->hy2));// ножка

        // геометрия отрисовки имени
        tSize = QSize(50,20);                               // макс.размер поля для номера
        bool left = (((int) subtype)%2) > 0;                // направление
        xyText = XY + (left ? QPointF(23, offset_y) : QPointF(offset_x - tSize.width(), offset_y));// точка написания номера
        boundRect = QRectF(xyText, tSize);
        // опции выравнивания текста
        option = new QTextOption((left ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignTop);

        center = XY + QPointF(prop->cx, prop->cy);          // центр круга светофора
        r = prop->d/2;                                      // радиус
    }
    else
    {
        log (QString("%1: %2").arg("Ошибка синтаксиса примитива СВЕТОФОР (ош.числа лексем)").arg(src));
    }
}

void ShapeSvtf::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}

// вычисление состояния примитива
void ShapeSvtf::accept()
{
    if (st == nullptr || (svtf==nullptr && svtfM==nullptr))
        state->set(Status::StsUndefined, true);             // неопред.состояние - нет справочников
    else
    {
        state->set(Status::StsUndefined, false);
        state->set(Status::StsExpire, st->IsTsExpire());

        state->set(StsOpenPzd , svtf  && svtf ->IsOpen());   // поездной
        state->set(StsOpenMnv , svtfM && svtfM->IsOpen());   // маневровый
        state->set(StsYellow  ,(svtf  && svtf ->IsYel()) || (svtfM  && svtfM ->IsYel()));   // маневровый не имеет желтого
        state->set(StsPrgls   , svtf  && svtf ->IsPrgls());  // пригласительный
        state->set(StsAD      , svtf  && svtf ->IsAd());     // автодействие светофора
        state->set(StsOM      , svtf  && svtf ->IsOm());     // отмена маршрута
        state->set(StsBlock   ,(svtf  && svtf ->IsBlocked()) || (svtfM  && svtfM ->IsBlocked()));
        state->set(StsPzdRoute, svtf  && svtf ->IsPzdRout());// в поездном маршруте
        state->set(StsMnvRoute, svtfM && svtfM->IsMnvRout());// в маневровом маршруте
        state->set(StsAlarm   ,(svtf  && svtf ->IsAlarm()) || (svtfM  && svtfM ->IsAlarm()));

        blinking = isAlarm() | isPrgls();
    }
}

// функция рисования
void ShapeSvtf::Draw(QPainter* painter)
{
    accept();


    QPen * pen1 = (svtf == nullptr && svtfM == nullptr) || state->isUndefined() ? PenUndefined : state->isExpire() ? PenExpired : MainPen;   // перо одинарной линии
    QPen * pen2 = (svtf == nullptr && svtfM == nullptr) || state->isUndefined() ? PenUndefined : state->isExpire() ? PenExpired : MainPen2;  // перо двойной линии

    bool compact = this->set->compactSvtf;

    // кисть основная
    QBrush * brush= state->isUndefined()                ? BrushUndefined:
                    state->isExpire ()                  ? BrushExpired  :  // нет данных
                    isOpenPzd       ()                  ? BrushPzdOn    :
                    isOpenMnv() && (isMnv() || compact) ? BrushMnvOn    :  // маневровый открыт и светофор маневровый, либо режим совмещения при закрытом поездном
                    isPrgls         ()                  ? (DShape::globalPulse ? BrushMnvOn : BrushPzdOff) :
                    isYellow        ()                  ? BrushYelllow  :
                    svtf && svtf->IsTypeIn()            ? BrushPzdInOff :
                                                          BrushPzdOff;
    // кисть маневровая
    QBrush * brushM = svtfM==nullptr || state->isUndefined() ? BrushUndefined :
                      isOpenMnv()                            ? BrushMnvOn     :
                                                              BrushMnvOff;

    // основание
    //painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setPen(*pen2);
    painter->drawLine(base);
    painter->setPen(*pen1);
    painter->drawLine(stand);

    // эллипс с заливкой
    // положит.эффект от включения Antialiasing особенно заметно при отрисовке окружностей в масштабе 2:1 и более
    // в масштабе 1:1 имеем даже незначительные отрицательные артефакты
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(*brush);
    switch (subtype)
    {
        // ножки - выходим
        case Base_E :
        case Base_NE:
        case Base_N :
        case Base_NW:
        case Base_W :
        case Base_SW:
        case Base_S :
        case Base_SE:
            break;

        // без ножки
        case Pzdn:
        case Mnvr:
            painter->drawEllipse(center, r, r);
            break;

        // повторители TO DO !!!
        case RepeteLR:
        case RepeteRL:
            painter->drawEllipse(center, r, r);

        // совмещенный слева направо
        case PzdnMnvrLR:
            if (compact)
                painter->drawEllipse(center, r, r);
            else
            {
                painter->drawEllipse(QPointF(2*r,0) + center, r, r);
                painter->setBrush(*brushM);
                painter->drawEllipse(center, r, r);
            }
            break;

        // совмещенный справа налево
        case PzdnMnvrRL:
            if (compact)
                painter->drawEllipse(center, r, r);
            else
            {
                painter->drawEllipse(QPointF(-2*r,0) + center, r, r);
                painter->setBrush(*brushM);
                painter->drawEllipse(center, r, r);
            }
            break;

        // одинарные поездные и маневровые
        case PzdnLR:
        case MnvrLR:
        case PzdnRL:
        case MnvrRL:
            painter->drawEllipse(center, r, r);
            break;
    }

    // наименование
    if (name.length())
    {
        // если не инициализировали подложку, инициализируем
        if (backRect.width() < 1)
            backRect = painter->boundingRect(boundRect, name, *option) + QMargins(2,0,3,2);


        QBrush * backBrush = state->isExpire() || state->isUndefined() ? nullptr :
                             isAD   () ? BrushAdBackground   :  // автодействие светофора выделяем фоном
                             isOM   () ? BrushOmBackground   :  // отмену маршрута выделяем фоном
                             isBlock() ? BrushLockBackground :  // заблокирован
                             isAlarm() ? BrushAlarmBackground:  // авария выделяется фоном подложки
                                         nullptr;

        if (backBrush)
            painter->fillRect(backRect, *backBrush);

        // текст
        painter->setFont(*font);
        if (!(state->isExpire() || state->isUndefined()))
            painter->setPen(*PenText);
        painter->drawText(boundRect, name, *option);
    }

    // мигание перекрестием
    if (isAlarm() && DShape::globalPulse && !state->isExpire() && !state->isUndefined())
    {
        painter->setPen(*PenAlarm);
        painter->drawLine(center + QPointF(-10, -10), center + QPointF(10, 10));
        painter->drawLine(center + QPointF(-10, +10), center + QPointF(10, -10));
    }

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

// геометрия светофоров по типам
ShapeSvtf::SvtfPropTag ShapeSvtf::svtfProp[MaxSvtfType] =
{
//    основание       ножка      центр и диаметр
// было: 1    16
    { 0 ,2,0 ,15,	0 ,8,5 ,8,      13,8,17 } ,             // 0    |--  поездной       Left To Right
    { 21,2,21,15,	16,8,21,8,	    8 ,8,17 } ,             // 1    --|  поездной       Right To Left
    { 0 ,2,0 ,15,	0 ,8,5 ,8,      13,8,17 } ,             // 2    |--  маневровый     Left To Right
    { 21,2,21,15,	16,8,21,8,	    8 ,8,17 } ,             // 3    --|  маневровый     Right To Left
    { 0 ,2,0 ,15,	0 ,8,5 ,8,      13,8,17 } ,             // 4    |--  совмещенный    Left To Right
    { 21,2,21,15,	16,8,21,8,	    8 ,8,17 } ,             // 5    --|  совмещенный    Right To Left
    { 0 ,2,0 ,15,	0 ,8,6 ,8,      14,8,17 } ,             // 6    |--  повторитель    Left To Right
    { 21,2,21,15,	16,8,21,8,	    8 ,8,17 } ,             // 7    --|  повторитель    Right To Left

    { 0,0,0,0,	    0,0,0,0,        12,12,17 },             // 8 поездной без ножки
    { 0,0,0,0,	    0,0,0,0,        12,12,17 },             // 9 маневровый без ножки

                                                            //      E N W S
    { 0 ,5 ,0 ,17,	0 ,11,4 ,11,    0,0,0 },                // 10   E   |-- основание слева
    { 0 ,18,11,23,	6 ,20,7	,18,    0,0,0 },                // 11	NE
    { 5 ,23,18,23,	11,23,11,19,    0,0,0 },                // 12	N   _|_ основание снизу
    { 12,23,23,18,	16,18,17,20,    0,0,0 },                // 13	NW  /
    { 23,5 ,23,17,	23,11,19,11,    0,0,0 },                // 14	W   --| основание справа
    { 12,0 ,23,5 ,	16,5 ,17,3 ,    0,0,0 },                // 15	SW
    { 5 ,0 ,18,0 ,	11,0 ,11,4 ,    0,0,0 },                // 16	S   --	основание сверху
    { 0 ,5 ,11,0 ,	6 ,3 ,7	,5 ,    0,0,0 },                // 17   SE  /
};
