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

QFont * ShapeSvtf::font;                                    // шрифт отрисовки названия


ShapeSvtf::ShapeSvtf(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    svtf  = nullptr;                                        // светофор поездной
    svtfM = nullptr;                                        // светофор совмещенный
    prop  = nullptr;

    Parse(src);
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

    PenUndefined       = new QPen (*BrushUndefined, 1);                                 // одинарная линия отрисовки неопред. состояния светофора
    PenExpired         = new QPen (*BrushExpired, 1);                                   // одинарная линия отрисовки состояния светофора при устаревании ТС
    MainPen            = new QPen (QBrush(colorScheme->GetColor("SvtfLinesNormal")), 1);// одинарная линия отрисовки светофора
    MainPen2           = new QPen (QBrush(colorScheme->GetColor("SvtfLinesNormal")), 2);// двойная линия отрисовки светофора
    PenAlarm           = new QPen (QBrush(colorScheme->GetColor("SvtfAlarmCross")), 1);// авария светофора

    font = new QFont("Segoe UI",10);
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
        if (idSvtf1 && (p=Svtf::GetById(idSvtf1)))
        {
            svtf  = !p->IsTypeMnv() ? p : nullptr;
            svtfM = p->IsTypeMnv() ? p : nullptr;
        }
        if (idSvtf2 && (p=Svtf::GetById(idSvtf2)))
        {
            svtf  = !p->IsTypeMnv() ? p : nullptr;
            svtfM = p->IsTypeMnv() ? p : nullptr;
        }
        prop = &svtfProp[subtype];

        // геометрия отрисовки имени
        tSize = QSize(50,20);                               // макс.размер поля для номера
        bool left = (((int) subtype)%2) > 0;                // направление
        xyText = XY + (left ? QPointF(23, 0) : QPointF(-2 - tSize.width(), 0));// точка написания номера
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
//        if (svtf ->IsOpen())
//            int a = 99;
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

    // Кисть основная
    QBrush * brush= state->isUndefined()                ? BrushUndefined:
                    state->isExpire ()                  ? BrushExpired    :  // нет данных
                    isOpenPzd       ()                  ? BrushPzdOn    :
                    isOpenMnv       ()                  ? BrushMnvOn    :   //   StsAct[StsOpenMnv] && (IsMnvrDraw || compact) ? BrushMnvOn :
                    isPrgls         ()                  ? (DShape::globalPulse ? BrushMnvOn : BrushPzdOff) :
                    isYellow        ()                  ? BrushYelllow  :
                    svtf && svtf->IsTypeIn()            ? BrushPzdInOff :
                                                          BrushPzdOff;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(*MainPen);
    painter->setBrush(*brush);
    painter->drawEllipse(center, r, r);
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
    { 0 ,1,0 ,16,	0 ,8,5 ,8,      13,8,17 } ,             // 0    |--  поездной       Left To Right
    { 21,1,21,16,	16,8,21,8,	    8 ,8,17 } ,             // 1    --|  поездной       Right To Left
    { 0 ,1,0 ,16,	0 ,8,5 ,8,      13,8,17 } ,             // 2    |--  маневровый     Left To Right
    { 21,1,21,16,	16,8,21,8,	    8 ,8,17 } ,             // 3    --|  маневровый     Right To Left
    { 0 ,1,0 ,16,	0 ,8,5 ,8,      13,8,17 } ,             // 4    |--  совмещенный    Left To Right
    { 21,1,21,16,	16,8,21,8,	    8 ,8,17 } ,             // 5    --|  совмещенный    Right To Left
    { 0 ,1,0 ,16,	0 ,8,6 ,8,      14,8,17 } ,             // 6    |--  повторитель    Left To Right
    { 21,1,21,16,	16,8,21,8,	    8 ,8,17 } ,             // 7    --|  повторитель    Right To Left

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
