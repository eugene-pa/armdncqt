#include "shapeset.h"
#include "shaperc.h"

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
        // 0- примитив
        type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
        // 1- номер станции
        idst = lexems[1].toInt(&ret);               ok &= ret;
        // 2- тип отрезка: для наклонной линии 0-"/"  1-"\"
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
        if (!LinkedStrl::checkList(strl, set->logger()))
            log (QString("Ошибка описания определяющих стрелок примитва: %1").arg(src));

        // 3-6- координаты
        x1    = lexems[3].toFloat(&ret);            ok &= ret;
        y1    = lexems[4].toFloat(&ret);            ok &= ret;
        x2    = lexems[5].toFloat(&ret);            ok &= ret;
        y2    = lexems[6].toFloat(&ret);            ok &= ret;
        setDimensions ();

        // номер РЦ
        idObj = lexems[7].toInt(&ret);              ok &= ret;

        // направляющие стрелки
        for (int i=8; i<11; i++)
        {
            int nostrl = nostrl = lexems[i].toInt(&ret);
            if (nostrl && ret)
                strl.append(new LinkedStrl(nostrl));
            ok &= ret;
        }

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        sprRc   = Rc::GetById(idObj);                       // РЦ

        if (!ok)
        {
            log (QString("Ошибка синтаксиса примитива: %1").arg(src));
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

// проверка нахождения определяющих стрелок в требуемом положении
bool ShapeRc::isStrlOk()
{
    return LinkedStrl::checkRqSts(strl)==0;
}

// содержимое этой функции логично сделать членом класса Route
// проверка, удовлетворяет ли положение направляющих стрелок отрезка ЗАДАННОМУ положению стрелок указанного маршрута
// функция используется для прокладки трассы устанавливаемого маршрута, при этом фактическое положение стрелок может отличаться от заданного
bool ShapeRc::isStrlInRoute(Route* route)
{
    foreach (LinkedStrl* link, strl)
    {
        foreach (LinkedStrl* routeLink, route->GetStrlList())
        {
            if (link->strl->Id() == routeLink->strl->Id() && link->rqStatus() != routeLink->rqStatus())
                return false;
        }
    }
    return true;
}

// вычисление состояния примитива
void ShapeRc::accept()
{
    state->set(StsRqRoute, false);
    if (st == nullptr || sprRc == nullptr)
        state->set(Status::StsUndefined, true);             // неопред.состояние - нет справочников
    else
    {
        state->set(Status::StsUndefined, false);
        if (st->IsTsExpire())
            state->set(Status::StsExpire, true);            // не данных - устарели ТС
        else
        {
            state->set(Status::StsExpire, false);
            // учитываем положение стрелок
            bool strok = isStrlOk();                        // акт.маршрут или nullptr
            state->set(StsBusy, sprRc->StsBusy() && strok); // занятость
            state->set(StsZmk , sprRc->StsZmk () && strok); // замыкание
            state->set(StsIr  , sprRc->StsIr  () && strok); // ИР

            Route * route = sprRc->ActualRoute();
            // устанавливаемый маршрут - не по фактическому, а по требуемому положению направляющих стрелок
            if (route != nullptr && route->Sts() == Route::RQSET)
            {
                if (isStrlInRoute(route))
                    state->set(StsRqRoute, true);           // устанавливается (тип здесь неважен)
            }

            if (route != nullptr && strok)
            {
                state->set(StsPassed  , sprRc->StsPassed());
                if (!sprRc->StsPassed() && route->Sts() != Route::RQSET)
                {
                    if (route->IsManevr())
                        state->set(StsMnvRoute, true);      // маневровый
                    else
                        state->set(StsPzdRoute, true);      // поездной
                }
            }
            else
            {
                state->set(StsPzdRoute, false);
                state->set(StsMnvRoute, false);
                state->set(StsPassed  , false);
            }
        }
    }
}

// функция рисования
void ShapeRc::Draw(QPainter* painter)
{
    // строго говоря, функция accept() не должна вызываться при отрисовке, она должна вызываться при обновлении данных
    // можно рассмотреть вопрос о синхронизации обновлений данных с обновлением статуса примитивов,
    // например: подписать на обновление станции статическую функцию, обновляющую все примитивы станции
    // уши: надо отслеживать срабатывание таймеров
    accept();

    blinking = isIr();

    // если е было изменений и не надо мигать - выход - не работает в QT
//    if (!state->hasChanges() && !blinking)
//        return;

    // если толщина линий не соответствует требуемой - изменить инструменты
    if (PenFree->width() != DShape::mThick)
        InitInstruments();

    QPen *pen =
            sprRc == nullptr                        ?   PenUndefined    :       // не привязана
            state->isExpire ()                      ?   PenExpired      :       // нет данных
            isIr() && DShape::globalPulse           ?   PenIr           :       // ИР в активной фазе
            sprRc->StsBusy()                        ?   PenBusy         :       // занята
            isPzdRoute () && !isPassed()            ?   PenPzdRoute     :       // в поездном и не пройдена
            isMnvRoute () && !isPassed()            ?   PenMnvRoute     :       // в маневровом и не пройдена
            isRqRoute  ()                           ?   PenRqRoute      :       // устанавливается маршрут
            isZmk      ()                           ?   PenZmk          :       // замкнута
            state->isUndefined()                    ?   PenUndefined    :
                                                        PenFree;

    QColor color(Qt::black);

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
