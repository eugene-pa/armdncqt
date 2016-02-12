#include "shapeset.h"
#include "shapestrl.h"
//#include "colorscheme.h"
//#include "../spr/station.h"

QBrush * ShapeStrl::BrushNormal;                            // кисть для отрисовки номера стрелки в норм.состоянии

QPen * ShapeStrl::PenFree;                                  // свободное состоняние
QPen * ShapeStrl::PenBusy;                                  // занятая стрелка
QPen * ShapeStrl::PenOtuRect;                               // желтое перо окантовки (ОТУ)
QPen * ShapeStrl::PenRqRoute;                               // в устанавливаемом маршруте
QPen * ShapeStrl::PenPzdRoute;                              // в поездном маршруте
QPen * ShapeStrl::PenMnvRoute;                              // в маневровом маршруте
QPen * ShapeStrl::PenZmk;                                   // замкнутая РЦ не в неиспользованном маршруте
QPen * ShapeStrl::PenIr;                                    // искусственная разделка (мигает поверх других состояний)
QPen * ShapeStrl::PenMuRect;                                // окантовка стрелки на МУ
QPen * ShapeStrl::PenUndefined;                             // объект неопределен - пассивная отрисовка
QPen * ShapeStrl::PenNormalOk;                              // тонкая линия нормали, корректное состояние
QPen * ShapeStrl::PenNormalAlarm;                           // тонкая линия нормали, взрез
QPen * ShapeStrl::PenAlarmPulse1;                           // авария (1-я фаза мигания)
QPen * ShapeStrl::PenAlarmPulse2;                           // авария (2-я фаза мигания)
QPen * ShapeStrl::PenIzsRound;                              // перо замыкания / блокировки
QPen * ShapeStrl::PenExpired;                               // ТС устарели

QFont * ShapeStrl::font;                                    // шрифт отрисовки названия



ShapeStrl::ShapeStrl(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    plusNormal = false;
    idrc = 0;
    b45 = false;

    sprStrl = nullptr;                                      // стрелка
    sprRc   = nullptr;                                      // РЦ под стрелкой
    prop    = nullptr;                                      // описание актуального набора геометрии

    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeStrl. Исключение при разборе строки: %1").arg(src));
    }
}

ShapeStrl::~ShapeStrl()
{

}

// инициализация статических инструментов отрисовки
void ShapeStrl::InitInstruments()
{
    BrushNormal     = new QBrush(colorScheme->GetColor("StrlNormal"           ));         // кисть для отрисовки номера стрелки в норм.состоянии

    PenFree         = new QPen (*BrushNormal, mThick);                                    // свободное состоняние
    PenBusy         = new QPen (QBrush(colorScheme->GetColor("Busy"           )), mThick);// занятая стрелка
    PenOtuRect      = new QPen (QBrush(colorScheme->GetColor("StrlOtuRect"    )), 2     );// желтое перо окантовки (ОТУ)
    PenIzsRound     = new QPen (QBrush(colorScheme->GetColor("StrlIszRound"   )), 2     );// перо замыкания / блокировки
    PenRqRoute      = new QPen (QBrush(colorScheme->GetColor("RouteSet"       )), mThick);// в устанавливаемом маршруте
    PenPzdRoute     = new QPen (QBrush(colorScheme->GetColor("RoutePzd"       )), mThick);// в поездном маршруте
    PenMnvRoute     = new QPen (QBrush(colorScheme->GetColor("RouteMnv"       )), mThick);// в маневровом маршруте
    PenZmk          = new QPen (QBrush(colorScheme->GetColor("Zmk"            )), mThick);// замкнутая РЦ не в неиспользованном маршруте
    PenIr           = new QPen (QBrush(colorScheme->GetColor("Ir"             )), mThick);// искусственная разделка (мигает поверх других состояний)
    PenMuRect       = new QPen (QBrush(colorScheme->GetColor("StrlMuRect"     )), 1     );// окантовка стрелки на МУ
    PenUndefined    = new QPen (QBrush(colorScheme->GetColor("Undefined"      )), mThick);// объект неопределен - пассивная отрисовка
    PenAlarmPulse1  = new QPen (QBrush(colorScheme->GetColor("StrlAlarmPulse1")), mThick);// авария (1-я фаза мигания)
    PenAlarmPulse2  = new QPen (QBrush(colorScheme->GetColor("StrlAlarmPulse2")), mThick);// авария (2-я фаза мигания)
    PenNormalOk     = new QPen (QBrush(colorScheme->GetColor("Normal"         )), 1     );// тонкая линия нормали, корректное состояние
    PenNormalAlarm  = new QPen (QBrush(colorScheme->GetColor("StrlAlarmPulse1")), 3     );// тонкая линия нормали, взрез
    PenExpired      = new QPen (QBrush(colorScheme->GetColor("Expired"        )), mThick);// ТС устарели

    //font = new QFont("MS Sans Serif",14);
    //font = new QFont("Arial",11);       // можно читать из таблицы граф.конфигурации из БД
    font = new QFont("Segoe UI",10);
}


// Разбор строки текстового описания примитива STRL
// 0  1  2   3    4   5 6   7     8  9  10  11  12    13   14
//    Ст Тип X1  Y1  X2 Y2  No    S1 S2 S3 отв  РЦ   Имя  Градус
// 19 2  2   1233 522 0 0   15    0  0  0   0   45    $3
// 19 17 3   494  217 0 0   3109  0  0  0   0   3081  $    45
// 2 - тип-форма стрелки
// 11- нормальное полложение (0 - по основному ходу, 1 - ответвление)
// 13- $[имя стрелки] - имя может быть пустым, остается только символ $
// 14- [45] - угол наклона 45 градусов, в случае отсутствия по умолчанию 60
void ShapeStrl::Parse(QString& src)
{
    bool ok  = true,
         ret = false;
    QStringList lexems = src.split(' ',QString::SkipEmptyParts);
    if (lexems.length() >= 14)
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

        // вторая пара координат нужна для универсальных вычислений
        if (x2 == 0 && y2 == 0)
        {
            x2 = x1 + 16;
            y2 = y1 + 16;
        }
        setDimensions ();
        rect = QRectF(x1, y1-8, 16, 16);                // сдвигаем окантовку

        // #стрелка
        idObj = lexems[7].toInt(&ret);                 ok &= ret;

        // 8-10 - направляюшие стрелки
        for (int i=8; i<11; i++)
        {
            int nostrl = nostrl = lexems[i].toInt(&ret);
            if (nostrl && ret)
                strl.append(new LinkedStrl(nostrl));
            ok &= ret;
        }
        if (!LinkedStrl::checkList(strl, set->logger()))
            log (QString("Ошибка описания определяющих стрелок примитва: %1").arg(src));

        // 11- нормальное полложение (0 - по основному ходу, 1 - ответвление)
        int normal = lexems[11].toInt(&ret);            ok &= ret;
        plusNormal = normal ==0 ? true : false;             // TRUE - плюс по основному ходу, FALSE - ответвление

        // 12 - #РЦ
        idrc    = lexems[12].toInt(&ret);               ok &= ret;

        // 13 - символьное имя стрелки (надпись)
        name    = lexems[13].replace("$","");               // имя (номер стрелки)


        // 14 - наклон
        if (lexems.length() > 14)                           // наклон
            b45 = lexems[12] == "45";

        // объектная привязка
        st      = Station::GetById(idst);                   // станция
        sprStrl = Strl::GetById(idObj);                     // стрелка
        sprRc   = Rc::GetById(idrc);                        // РЦ
        if (sprRc != nullptr && sprStrl != nullptr)
            sprStrl->SetRc(sprRc);                          // Неочевидное поведение: ссылки SprRc и SprRc2 класса StrlInfo устанавливаются ТОЛЬКО ПРИ ЧТЕНИИ ФОРМ

        prop = b45 ? &strlProp45[subtype] : &strlProp60[subtype];

        // место отрисовки текста
        tSize = QSize(50,20);                               // размер поля для номера
        xyText = XY + QPointF(prop->xt, prop->yt - 2);
        if (prop->align == _TA_RIGHT)
            xyText -= QPoint(tSize.width(),0);
        // опции выравнивания текста
        option = new QTextOption((prop->align==_TA_LEFT ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignTop);

        // Геометрия отрисовки стрелки в плюсе
        pathForPlus.append(XY + QPointF(prop->x1,prop->y1));
        pathForPlus.append(XY + QPointF(prop->x0,prop->y0));
        pathForPlus.append(XY + (plusNormal ? QPointF(prop->x2,prop->y2) : QPointF(prop->x3,prop->y3)));

        // Геометрия отрисовки стрелки в минусе
        pathForMinus.append(XY + QPointF(prop->x1,prop->y1));
        pathForMinus.append(XY + QPointF(prop->x0,prop->y0));
        pathForMinus.append(XY + (!plusNormal ? QPointF(prop->x2,prop->y2) : QPointF(prop->x3,prop->y3)));

        // Геометрия отрисовки нормального положения по ходу
        pathNormalPlus.append(XY + QPointF(prop->x1P,prop->y1P));
        pathNormalPlus.append(XY + QPointF(prop->x2P,prop->y2P));

        // Геометрия отрисовки нормального положения на ответвление
        pathNormalMinus.append(XY + QPointF(prop->x1O,prop->y1O));
        pathNormalMinus.append(XY + QPointF(prop->x2O,prop->y2O));

        // дял отрисовеи окантовки расширяем прямоугольник rect на dxy пикселей
        int dxy = 7;
        boundRect = QRect(rect.x()-dxy, rect.y()-dxy, width + dxy*2, height + dxy*2);

        if (!ok)
        {
            log (QString("%1: %2").arg("Ошибка синтаксиса примитива").arg(src));
        }
    }
    else
    {
        log (QString("%1: %2").arg("Ошибка синтаксиса примитива СТРЕЛКА (ош.числа лексем)").arg(src));
    }
}


// при отрисовке вызывается вирт.функция paint, которая вызывает функцию непосредственной отрисовки Draw(QPainter* painter)
void ShapeStrl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}

// вычисление состояния примитива
void ShapeStrl::accept()
{
    if (st == nullptr || sprRc == nullptr || sprStrl == nullptr)
        state->set(Status::StsUndefined, true);             // неопред.состояние - нет справочников
    else
    {
        state->set(Status::StsUndefined, false);
        state->set(Status::StsExpire, st->IsTsExpire());
        state->set(StsPlus , sprStrl->stsPls);
        state->set(StsMinus, sprStrl->stsMns);
        state->set(StsIzs  , sprStrl->stsZs || sprStrl->stsZpr);// искусственное замыкание, блокировка
        state->set(StsOtu  , sprStrl->stsOtu);                // стрелка выбрана для ОТУ вспомогат.перевода
        state->set(StsMu   , sprStrl->stsMu);
        state->set(StsAlarm, sprStrl->stsVzres || (sprStrl->stsPls == sprStrl->stsMns));

        bool strok = isStrlOk();                            // вхождение по направляющим стрелкам

        // здесь нельзя использовать свойства стрелки занятость, замыкание, так как для спаренных стрелок неизвестно, какая первая
        // надо брать конкретную РЦ

        state->set(StsBusy, (sprRc==nullptr ? false : sprRc->StsBusy()) && strok);   // занятость РЦ с учетом направляющих стрелок
        state->set(StsZmk , (sprRc==nullptr ? false : sprRc->StsZmk ()) && strok);   // замыкание РЦ с учетом направляющих стрелок
        state->set(StsIr  , (sprRc==nullptr ? false : sprRc->StsIr  ()) && strok);   // разделка  РЦ с учетом направляющих стрелок

        // устанавливаемый маршрут - не по фактическому, а по требуемому положению направляющих стрелок
        state->set(StsRqRoute, false);
        if (ActualRoute() != nullptr && ActualRoute()->Sts() == Route::RQSET)
        {
            if (ActualRoute()->CheckRqState(strl))
                state->set(StsRqRoute, true);               // устанавливается (тип здесь неважен)
        }

        if (ActualRoute() != nullptr && strok)
        {
            state->set(StsPassed, sprRc->StsPassed());
            if (!sprRc->StsPassed() && ActualRoute()->Sts() != Route::RQSET)
            {
                if (ActualRoute()->IsManevr())
                    state->set(StsMnvRoute, true);          // маневровый
                else
                    state->set(StsPzdRoute, true);          // поездной
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


// функция рисования
void ShapeStrl::Draw(QPainter* painter)
{
    accept();

    blinking = isIr() || isAlarm() || isOtu();

    // 1. отрисовка непосредственно стрелки
    QPen *pen =
            state->isUndefined()                    ?   PenUndefined    :
            state->isExpire ()                      ?   PenExpired      :       // нет данных
            isAlarm()                               ?   (DShape::globalPulse ? PenAlarmPulse1 : PenAlarmPulse2) :
            isIr() && DShape::globalPulse           ?   PenIr           :       // ИР в активной фазе
            isBusy()                                ?   PenBusy         :       // занята
            isPzdRoute () && !isPassed()            ?   PenPzdRoute     :       // в поездном и не пройдена
            isMnvRoute () && !isPassed()            ?   PenMnvRoute     :       // в маневровом и не пройдена
            isZmk      ()                           ?   PenZmk          :       // замкнута
            isRqRoute  ()                           ?   PenRqRoute      :       // устанавливается маршрут
                                                        PenFree;
    painter->setPen(*pen);
    if (state->isUndefined() || isPlus() == isMinus())
    {
        painter->drawPolyline(pathForPlus);
        painter->drawPolyline(pathForMinus);
    }
    else
    {
        painter->drawPolyline(isPlus() ? pathForPlus : pathForMinus);
    }

    // 2. отрисовка штриха нормального (плюсового) положения
    if (!state->isUndefined() && !state->isExpire ())
        painter->setPen(isAlarm() ? *PenNormalAlarm : *PenNormalOk);
    painter->drawPolyline(plusNormal ? pathNormalPlus : pathNormalMinus);


    // 3. окантовки рисуем только при наличии актуальных данных
    if (!state->isUndefined() && !state->isExpire ())
    {
        if (sprStrl && sprStrl->Disabled())                 // законсервирована
            drawRect(painter, *PenIzsRound);
        if (isMu())                                         // МУ
            drawRect(painter, *PenMuRect);
        if (isMu())                                         // МУ
            drawRect(painter, *PenMuRect);
        if (isIzs())                                        // искусственно замкнута
            drawRect(painter, *PenIzsRound);
        if (isOtu())                                        // ОТУ (можно мигать как сейчас. например, менять прозрачность)
            drawRect(painter, *PenOtuRect);
    }

    // 4. имя стрелки; перо меняем если данные актуальные
    if (!state->isUndefined() && !state->isExpire ())
    {
        pen = isAlarm           ()  ? PenBusy         :     // №стрелкм: авария - красный
              isOtu             ()  ? PenOtuRect      :     // ОТУ - желтый
              isIzs             ()  ? PenBusy         :     // искусств.замыкание - красный
              isMinus           ()  ? PenOtuRect      :     // по минусу - желтый  (2016.02.09)
                                      PenFree;              // в норме - черный,
    }
    painter->setPen(*pen);
    font->setWeight(pen == PenFree ? 50 : 70);
    painter->setFont(*font);
    painter->drawText(QRectF(xyText, tSize), name, *option);
}


// отрисовка квадратной окантовки стрелки
// если есть основания мигать - мигаем
void ShapeStrl::drawRect(QPainter* painter, QPen pen)
{
    if (!blinking || DShape::globalPulse)
    {
        painter->setPen(pen);
        painter->drawRect(boundRect);
    }
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

// описание геометрии стрелок 60 град.
StrlShapeProp ShapeStrl::strlProp60[nMaxStrlType]=
{
    // Базовые стрелки
    { _TA_RIGHT,8 ,-16		, 8 ,  4, 16,  4,		6, -4,  10, -12 ,	0,0,	8,0,	16,0,	12,-8,	},
    { _TA_LEFT ,8 ,-16		, 8 ,  4, 0,   4,		6, -12, 10, -4  ,	16,0,	8,0,	0,0,	4 ,-8,	},
    { _TA_RIGHT,8 , 1		, 8 , -4, 16, -4,		6, +4,  10, +12 , 	0,0,	8,0,	16,0,	12,8,	},
    { _TA_LEFT ,8 , 1		, 8 , -4, 0,  -4,		6, +12, 10, +4  , 	16,0,	8,0,	0,0,	4,8,	},

    // Примитивы с базовой наклонной линией
    { _TA_RIGHT,3 ,-16		, 2, -3,   6,-11,		6 ,  4, 14,  4  , 	0,8,	4,0,	8,-8,	12,0,	},
    { _TA_LEFT ,13,-16		, 14,-3,  10, -11,		3 ,  4, 11,  4  , 	16,8,	12,0,	8,-8,	4,0,	},
    { _TA_RIGHT,3,  0		, 2, +3,   6,+11,		6 , -4, 14, -4  , 	0,-8,	4,0,	8,8,	12,0,	},
    { _TA_LEFT ,12, 0		, 14,+3,  10, +11,		3 , -4, 11, -4  , 	16,-8,	12,0,	8,8,	4,0,	},

    // Примитивы с базовой вертикальной линией
    { _TA_LEFT ,12, -5		, 4 , -2, 4,  -12,		13, -2,  17, -10 , 	8,8,	8,0,	8,-12,	14,-12,	},
    { _TA_RIGHT,4 , -5		, 12, -2, 12, -12,		4 , -2,  -1, -10 , 	8,8,	8,0,	8,-12,	2,-12,	},
    { _TA_LEFT ,12,-10		, 4 , +2, 4,  +12,		13, +2,  17, +10 , 	8,-8,	8,0,	8,12,	14,12,	},
    { _TA_RIGHT,4 ,-10		, 12, +2, 12, +12,		4 , +2,  -1, +10 , 	8,-8,	8,0,	8,12,	2,12,	},

    // горизонтальные разветвления
    { _TA_LEFT ,12, -7		, 5 , -4, 9,  -12,		5 , +4, 9,  +12  , 	0,0,	8,0,	12,-8,	12,+8,	},
    { _TA_RIGHT,4 , -7		, 11, -4, 7,  -12,		11, +4, 7,  +12  , 	16,0,	8,0,	4,-8,	4,+8,	},

    // Ответвления от наклонной вверх и вниз
    { _TA_RIGHT,0,  -8		, 8 ,  0,12,   -8,       0, -1, 0,  -10  ,	0,8,	4,0,	8,-8,	4,-10,	},	// 14
    { _TA_LEFT ,16 ,-8		, 8 ,  0, 4,   -8,       15,-1,15,  -10  , 	16,8,	12,0,	8,-8,	12,-10,	},	// 15
    { _TA_RIGHT,6  ,-4		, 16,  0,20,   +8,       8,  1, 8,  +10  , 	8,-8,	12,0,	16,8,	12,10,	},	// 16
    { _TA_LEFT ,10 ,-4		, 0 ,  0,-4,   +8,       7,  1, 7,  +10  , 	8,-8,	4,0,	0,8,	4,10,	},	// 17

};

// описание геометрии стрелок 45 град.
StrlShapeProp ShapeStrl::strlProp45[nMaxStrlType]=
{
    // Базовые стрелки
    { _TA_RIGHT,8 ,-16		, 8 ,  4, 16,  4,		6, -4,  14, -12 ,	0,0,	8,0,	16,0,	16,-8,	},
    { _TA_LEFT ,8 ,-16		, 8 ,  4, 0,   4,		2, -12, 10, -4  ,	16,0,	8,0,	0,0,	0 ,-8,	},
    { _TA_RIGHT,8 , 1		, 8 , -4, 16, -4,		6, +4,  14, +12 , 	0,0,	8,0,	16,0,	16,8,	},
    { _TA_LEFT ,8 , 1		, 8 , -4, 0,  -4,		2, +12, 10, +4  , 	16,0,	8,0,	0,0,	0,8,	},

    // Примитивы с базовой наклонной линией
    { _TA_RIGHT,7 ,-16		, 4, -2,  12,-10,		8 ,  4, 16,  4  , 	0,8,	8,0,	16,-8,	16,0,	},
    { _TA_LEFT ,11,-16		, 12,-2,  4, -10,		0 ,  4, 8,  4  , 	16,8,	8,0,	0,-8,	0,0,	},
    { _TA_RIGHT,5,  0		, 4, +2,  12,+10,		8 , -4, 16, -4  , 	0,-8,	8,0,	16,8,	16,0,	},
    { _TA_LEFT ,11, 0		, 12,+2,  4, +10,		0 , -4, 8, -4  , 	16,-8,	8,0,	0,8,	0,0,	},

    // Примитивы с базовой вертикальной линией
    { _TA_LEFT ,12, -4		, 4 , 0, 4,  -8,		12, 2,  20, -6 , 	8,8,	8,0,	8,-8,	16,-8,	},
    { _TA_RIGHT,4 , -4		, 12, 0, 12, -8,		4 , 2,  -4, -6 , 	8,8,	8,0,	8,-8,	0,-8,	},
    { _TA_LEFT ,12,-12		, 4 , 0, 4,  +8,		12, -2, 20,  6 , 	8,-8,	8,0,	8,8,	16,8,	},
    { _TA_RIGHT,4 ,-12		, 12, 0, 12, +8,		4 , -2, -4,  6 , 	8,-8,	8,0,	8,8,	0,8,	},

    // горизонтальные разветвления
    { _TA_LEFT ,13, -7		, 5, -3,  13,-11,		5, +3,  13, +11  , 	0,0,	8,0,	16,-8,	16,+8,	},
    { _TA_RIGHT,3 , -7		, 11,-4,  3, -12,		11,+4,  3,  +12, 	16,0,	8,0,	0,-8,	0,+8,	},

    // Ответвления от наклонной вверх и вниз
    { _TA_RIGHT,4,  -11		, 5, -3,  13,-11,       4, 0, 4,  -8  ,	0,8,	8,0,	16,-8,	8,-8,	},	// 14
    { _TA_LEFT ,12 ,-11		, 11,-4,  3, -12,       12,0,12,  -8  , 	16,8,	8,0,	0,-8,	8,-8,	},	// 15
    { _TA_RIGHT,4  ,-4		, 10,-3, 18,  +5,       4, 0, 4,  +8  , 	0,-8,	8,0,	16,8,	8,8,	},	// 16
    { _TA_LEFT ,12 ,-4		,-2 , 4,  6,  -4,       12,0,12,  +8  , 	16,-8,	8,0,	0,8,	8,8,	},	// 17

};
