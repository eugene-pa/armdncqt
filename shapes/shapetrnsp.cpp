#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRectF>
#include <QtWidgets>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "shapetrnsp.h"
#include "shapeset.h"
#include "../common/boolexpression.h"
#include "../spr/sprbase.h"

QVector<TrnspDescription *> TrnspDescription::descriptions;            // массив описателей
bool TrnspDescription::loaded = false;

ShapeTrnsp::ShapeTrnsp(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    // обнуление массива формул
    for (int i=0; i<maxStates; i++)
    {
        stsExpr[i] = stsPulseExpr[i] = nullptr;
    }
    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeTrnsp. Исключение при разборе строки: %1").arg(src));
    }

//  setDimensions ();
}

ShapeTrnsp::~ShapeTrnsp()
{

}

// Разбор строки текстового описания примитива MODE (TRNSP)
// 0   1  2     3    4      5    6
// #  Ст  id    x    y      w    h
// 20  2 50   1418  631     48   20    "!2УО&2НКП !2УО ~2УО"  1 2НСН "Смена направления"  0
// 20  2  7     71   88     40   24       "ДСН  "             2 ДСН  "Включение двойного снижения напряжения" ОДСН "Отмена двойного снижения напряжения" 0
// Поля 0-6 - см.выше
// Поля 7 и далее: описываем выражения для разных состояний (до трех состояний)
// В общем случае так:  "Выражение1[,Мигание1] [[Выражение2] Выражение3]" [~] число ТУ [[ТУ1 "Описание ТУ1"] ТУ2 "Описание ТУ2"] ПРИЗНАК_ПРИВЯЗКИ

void ShapeTrnsp::Parse(QString& src)
{
    bool ok   = true,
         ret  = false;

    // заменяю кавычки на кавычки с пробелом, чтобы отделить кавычки от лексем, и делаю разбор
    QStringList lexems = src.replace("\"", " \" ").split(' ',QString::SkipEmptyParts);
    if (lexems.count() < 7)
    {
        log (QString("%1: %2").arg("Ошибка синтаксиса примитива ТРАНСПАРАНТ (ош.числа лексем)").arg(src));
        return;
    }

    // 0 - тип
    type = (ShapeType)lexems[0].toInt(&ret);    ok &= ret;
    // 1 - номер станции
    idst = lexems[1].toInt(&ret);               ok &= ret;
    // 2  - тип транспаранта
    idObj= lexems[2].toInt(&ret);               ok &= ret;
    idObj++;                                                // приводим в соответствие нумерацию траспарантов в БД и SHAPE-файле

    // 3-6 - геометрия (нач.точка, ширина, высота)
    x1    = lexems[3].toFloat(&ret);            ok &= ret;
    y1    = lexems[4].toFloat(&ret);            ok &= ret;
    width = lexems[5].toFloat(&ret);            ok &= ret;
    height= lexems[6].toFloat(&ret);            ok &= ret;

    if (idObj == TRNSP_BUTTON)
        height -= 3;                                        // для башиловских "кнопок" поджимаем размер на 3 пиксела
    x2 = x1 + width;
    y2 = y1 + height;

    setDimensions ();


    // ищем описатель свойств и рассчитываем графику с учетом координат транспаранта
    prop = idObj>=0 && idObj<TrnspDescription::descriptions.count() ? TrnspDescription::descriptions[idObj] : nullptr;
    if (prop == nullptr)
        set->logger()->log(QString("ShapeTrnsp. Не найдено описание транспаранта с типом #%1: '%2'").arg(idObj).arg(src));
    else
    {
        if (prop->geometry.length() > 0)
            prop->MakePath(QPointF(x1,y1), path, set->logger());
        indicator = prop->nameTsDefault == "Индикатор";
    }

    enableTu = false;

    // объектная привязка
    st      = Station::GetById(idst);                       // станция

    if (lexems.length() < 8)                                // если вообще нет спецификации ТС - выход
        return;

    // 7 - сначала обработаем "частный случай" старого формата: всего 8 параметров, последний без кавычек)
    if (lexems[7] != "\"")                                  // 7 ТС1
    {
        // поддерживаем старый стиль описания единственного ТС без кавычек
        stsExpr[0] = new BoolExpression(lexems[7]);
        if (stsExpr[0]->Valid())
            QObject::connect(stsExpr[0], SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
        else
            log (QString("ShapeTrnsp. Ошибка выражения в описании транспаранта: %1").arg(src));
        return;
    }

    QString token;
    // имеем актуальную лексему с кавычками: " Выражение1[,Мигание1] [[Выражение2] Выражение3] "
    // ПРОБЛЕМА: Может быть и такое описание: " " - пробел внутри кавычек, надо уметь обработать
    int index = 8;
    int i = 0;
    while ((token = lexems[index++]) != "\"")
    {
        if (i >= maxStates)
        {
            log(QString("ShapeTrnsp. Превышно допустимое число выражений(<=%1)в поле ТС: '%2'").arg(maxStates).arg(src));
            return;
        }

        // ищем мигающее вражение
        QStringList sPulse = token.split(',');
        if (sPulse.length()> 0)
        {
            stsExpr[i] = new BoolExpression(sPulse[0]);
            if (stsExpr[i]->Valid())
                QObject::connect(stsExpr[i], SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
            else
                log (QString("ShapeTrnsp. Ошибка выражения в описании транспаранта: %1").arg(src));

            // если есть мигание - обработать
            if (sPulse.length()> 1)
            {
                stsPulseExpr[i] = new BoolExpression(sPulse[1]);
                if (stsPulseExpr[i]->Valid())
                    QObject::connect(stsPulseExpr[i], SIGNAL(GetVar(QString&,int&)), st, SLOT(GetValue(QString&,int&)));
                else
                    log (QString("ShapeTrnsp. Ошибка выражения в описании транспаранта: %1").arg(src));
            }
        }
    }

    if (index > lexems.length() - 1)    return;

    // проверяем опциональное наличие тильды [~]
    token = lexems[index++];
    if (token == "~")                                       // тильда после описания ТС - признак мигания
    {
        pulsing = true;
        if (index > lexems.length() - 1)    return;         // нет ТУ
        token = lexems[index++];
    }

    // число ТУ [[ТУ1 " Описание ТУ1 "] ТУ2 " Описание ТУ2 "] ПРИЗНАК_ПРИВЯЗКИ
    int ntu = token.toInt(&ret);    ok &= ret;
    if (ntu > 2)
    {
        log (QString("ShapeTrnsp. Некорректное число команд ТУ (%1)в описании примитива: : %2").arg(ntu).arg(src));
        ntu = 2;
    }

    //                                                                      0   1         2         3
    // обрабатываем команды формата: ТУ " Описание команды ТУ ", например: 2НСН " Смена направления "
    for (i = 0; i < ntu; i++)
    {
        tuNames.append(lexems[index++]);                    // ТУ
        index++;                                            // кавычка

        QString text;
        while ((token = lexems[index++]) != "\"")           // конкатенация строк до кавычки
            text += token + " ";
        tuTexts.append(text);
        enableTu = true;
    }

    if (index > lexems.length() - 1)    return;             //

    // далее может идти признак привязки транспаранта к следующему текстовому примитиву
    bool bLink = lexems[index++].toInt(&ret) != 0 ? true : false;
    if (index > lexems.length() - 1)    return;             //

    // далее может идти либо комментарий, либо описание транспаранта в кавычках
    token = lexems[index++];

    if (token == "\"")
    {
        // нашли кавычку, значит есть индивидуальное описание примитива
        while ((lexems[index] != "\"") && index < lexems.length()- 1)
            toolTipText += lexems[index++] + " ";

        if (index < lexems.length())
            index++;
        else
            log (QString("ShapeTrnsp. нет завершающего символа \" в примитиве: %1").arg(src));
    }
    if (index > lexems.length() - 1)    return;             //

    // если след.лексема начинается с символа ";", это комментарий
    if (lexems[index].indexOf(';') == 0)
    {
        while (index <= lexems.length() - 1)
            comment += lexems[index++] + " ";
    }

    // Перья и кисти транспаратнтов назначаются для
    //    XY_titleOn  = XY + new Vector((Width - TitleOn.Width)/2, (Height - TitleOn.Height)/2);
    //    XY_titleOff = XY + new Vector((Width - TitleOff.Width)/2, (Height - TitleOff.Height)/2);
    //    XY_titleExt = XY + new Vector((Width - TitleExt.Width)/2, (Height - TitleExt.Height)/2);

}


// инициализация статических инструментов отрисовки
void ShapeTrnsp::InitInstruments()
{
}

//// вычисление замещаемого прямоугольника
//void ShapeRc::GetArea(short*,short*,short*,short*)
//{

//}

bool ShapeTrnsp::CheckIt()
{
    return false;
}

void ShapeTrnsp::FixUpUnsafe()
{

}

void ShapeTrnsp::Prepare()
{
}

QString ShapeTrnsp::Dump()
{
    return "ТРАНСП";
}

QString  ShapeTrnsp::ObjectInfo()
{
    return "ТРАНСП";
}

// вычисление состояния примитива
void ShapeTrnsp::accept()
{
    state->set(StsOn , false);
    state->set(StsOff, false);
    state->set(StsExt, false);
    if (st == nullptr)
        state->set(Status::StsUndefined, true);             // неопред.состояние - некореектная привязка к станции
    else
    {
        state->set(Status::StsExpire, st->IsTsExpire());

        switch (idObj)
        {
            case TRNSP_CHDK:                                // состояние транспарантов ЧДК определяется состоянием перегонов. ОБРАБОТАТЬ!
                state->set(StsOff, true);
                return;

            case TRNSP_BLIND_L:                             // состояние траспарантов "слепой перегон" - всегда пассив
            case TRNSP_BLIND_R:
                state->set(StsOff, true);
                return;

            case TRNSP_TS:
                // состояние траспаранта ТС определяется наличием неопред. сигналов, устареванием и т.д.
                // надо уметь определять, есть ли в марице ненулевые сигналы на местах, не описанных в БД
                state->set(StsOff, true);
                if (st->IsUndefinedTsPresrnt())
                    state->set(StsExt, true);
                return;

            case TRNSP_KP:                                  // состояние КП
                // 1. Для МПЦ/РПЦ нужно корректно отработать ситуацию работоспособного КП и отсутствия связи с МПЦ/РПЦ
                // 2. При использовании транспаратнта на общем виде он интегрирует в себе функции транспарантов ТС,ТУ,ОТУ,Осн/Рзрв
                //    используя тртье состояние (желтый цвет)
                state->set(StsOff,  st->IsKpOk());
                state->set(StsOn , !st->IsKpOk());
                return;

            case TRNSP_MAIN_RSRV:                           // основной/резервный блок
                state->set(StsOff, !st->IsRsrv());
                state->set(StsOn ,  st->IsRsrv());
                return;

            case TRNSP_NGB:                                 // негабарит
                state->set(StsOff, true);
                return;

        }

//        Blinking = false;

//        // 2016.02.15. Не реализовано мигание по выражениям StsPulseExpr
//        StsAct[StsOn] = StsTsExpr[0] == null ? false : StsTsExpr[0].Value > 0;

//        if (!StsAct[StsOn])
//        {
//            StsAct[StsOff] = StsTsExpr[1] == null ? true : StsTsExpr[1].Value > 0;
//            if (!StsAct[StsOff])
//                StsAct[StsExt] = StsTsExpr[2] == null ? true : StsTsExpr[2].Value > 0;
//        }

//        // отдельно обрабатываем транспарант режимов управления
//        if (IsModeTransparant && SprSt != null)
//        {
//            StsAct[StsOn] = SprSt.Du; // состояние 1 - ДУ
//            StsAct[StsOff] = SprSt.Ru; // состояние 0 - РУ
//            StsAct[StsExt] = SprSt.Au || SprSt.Su; // состояние 2 - АУ или СУ
//        }

//        // реализация мигания с помощью расширения выражения через запятую, например: АДН,~АДН
//        // ВАЖНО: мигает то соcтояние транспаранта, в котором обнаружено выполнение выражения мигания
//        for (int i = 2; i >= 0; i--)
//        {
//            if (StsPulseExpr[i] != null && StsPulseExpr[i].Value > 0)
//            {
//                Blinking = true;
//                StsAct[i == 0 ? StsOn : i == 1 ? StsOff : StsExt] = true;
//            }
//        }
//        // Если транспарант должен пульсировать и StsAct[StsOn] - обеспечиваем пульсацию сменой состояния ON/OFF
//        // Используемый алгоритм пытается мигать между активным и пассивным состоянием, что не очень удобно
//        if (Pulsing && StsAct[StsOn] /*&& GlobalPulsingTrigger) || Blinking*/)
//        {
//            //StsAct[StsOn] = false;    // раньше перебрасывал состояния ОN/OFF
//            //StsAct[StsOff] = true;
//            Blinking = true;
//        }
    }
}

void ShapeTrnsp::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}

// функция рисования
void ShapeTrnsp::Draw(QPainter* painter)
{
    QPen pen(color1());
    pen.setWidth(1);
    pen.setCapStyle(Qt::FlatCap);
    painter->setPen(pen);

    QBrush brush(back1());
    painter->setBrush(brush);

    painter->setRenderHint(QPainter::Antialiasing);

    if (indicator)
    {
        painter->drawEllipse(rect);
    }
    else
    {
        if (path.length() != 0)
        {
//            Brush brush = //StsAct.StsExpire == true                ? BrushExpired  :
//                            StsAct[StsOn]                           ? BackBrushOn   :
//                            StsAct[StsOff]                          ? BackBrushOff  :
//                            StsAct[StsExt] && Property.IsThreeState ? BackBrushExt  :
//                            StsAct.StsUndefined                     ? BrushUndefined : null;

//            Pen pen = //StsAct.StsExpire == true                    ? PenExpired    :
//                        StsAct[StsOn]                               ? ForePenOn     :
//                        StsAct[StsOff]                              ? ForePenOff    :
//                        StsAct[StsExt] && Property.IsThreeState     ? ForePenExt    :
//                        StsAct.StsUndefined                         ? PenUndefined  : null;
//            painter->setPen(pen);
//            painter->setBrush(brush);
            painter->drawPath(path);
        }
        else
        {
            painter->drawRect(rect);
        }

        QTextOption op(Qt::AlignCenter);
        painter->drawText(rect, prop->name, op);
    }

    Q_UNUSED(painter)
}



// конструктор описаиеля транспаранта; добавляет указатель на класс в вектор описателей, при необходимости расширяя его размерность
TrnspDescription::TrnspDescription(int _id)
{
    id = _id;
    if (descriptions.size() < id+1)
        descriptions.resize(id+1);                          // увеличиваем размерность вектора
    descriptions[id] = this;
}

bool TrnspDescription::readBd(QString& dbpath, Logger& logger)
{
    logger.log(QString("Чтение свойств транспарантов: %1").arg(dbpath));
    loaded = true;
    QString sql("SELECT * FROM [TransparantsEx] ORDER BY [No]");

    try
    {
        QSqlDatabase dbSql = GetSqliteBd(dbpath);
        if (dbSql.open())
        {
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    bool ret;
                    // читаем имя параметра и RGBA  цвет для всех цветовых схем
                    int id = query.value("No").toInt(&ret);
                    TrnspDescription * d = new TrnspDescription(id);

                    // Даю возможность задать разные надписи для разных состояний транспаранта, например: ДУ;РУ;АУ;СУ
                    // Для этого в поле Text должны быть записаны лексемы в порядке следования состояний; всего 4 состояния
                    d->name = query.value("Text").toString().trimmed();
                    QStringList names = d->name.split(' ', QString::SkipEmptyParts);
                    if (names.length() > 0)
                        d->name = names[0];
                    if (names.length() > 1)
                        d->name2 = names[1];
                    if (names.length() > 2)
                        d->name3 = names[2];
                    if (names.length() > 3)
                        d->name4 = names[3];


                    d->nameTsDefault = query.value("Ts").toString().trimmed();
                    d->width  = query.value("W").toInt(&ret);
                    d->height = query.value("H").toInt(&ret);

                    // FORE ОN
                    d->foreColorOn  = QColor::fromRgb(query.value("On_ClrR").toInt(), query.value("On_ClrG").toInt(), query.value("On_ClrB").toInt());
                    d->foreBrushOn  = QBrush(d->foreColorOn);               // Кисть состояния ON
                    d->forePenOn    = QPen(d->foreColorOn);                 // ПЕро состояния ON
                    // BACK ОN
                    d->backColorOn  = QColor::fromRgb(query.value("On_BckR").toInt(), query.value("On_BckG").toInt(), query.value("On_BckB").toInt());
                    d->backBrushOn  = QBrush(d->backColorOn);				// Кисть переднего плана состояния ON
                    d->backPenOn    = QPen  (d->backColorOn);               // Перо  переднего плана состояния ON

                    d->foreColorOff = QColor::fromRgb(query.value("Off_ClrR").toInt(), query.value("Off_ClrG").toInt(), query.value("Off_ClrB").toInt());
                    d->backColorOff = QColor::fromRgb(query.value("Off_BckR").toInt(), query.value("Off_BckG").toInt(), query.value("Off_BckB").toInt());


                    // третье состояние
                    d->foreColorExt = QColor::fromRgb(query.value("Ex_ClrR").toInt(), query.value("Ex_ClrG").toInt(), query.value("Ex_ClrB").toInt());
                    d->backColorExt = QColor::fromRgb(query.value("Ex_BckR").toInt(), query.value("Ex_BckG").toInt(), query.value("Ex_BckB").toInt());

                    // если определено 4-е состояние в поле EX2RGB - разбор
                    // Формат: r1,g1,b1 r2,g2,b2 (1-цвет, 2-фон)
                    QString ext = query.value("EX2RGB").toString().trimmed();
                    if (ext.length() > 0)
                    {
                        QStringList rgbs = ext.split(' ', QString::SkipEmptyParts);
                        if (rgbs.length() == 2)
                        {
                            QStringList clr = rgbs[0].split(',', QString::SkipEmptyParts);
                            QStringList bck = rgbs[1].split(',', QString::SkipEmptyParts);
                            if (clr.length() == 3 && bck.length() == 3)
                            {
                                d->foreColorExt2 = QColor::fromRgb(clr[0].toInt(), clr[1].toInt(), clr[2].toInt());
                                d->backColorExt2 = QColor::fromRgb(bck[0].toInt(), bck[1].toInt(), bck[2].toInt());
                            }
                            else
                            {
                                logger.log(QString("Ошибка формата поля EX2RGB: %1").arg(ext));
                            }

                        }
                        else
                        {
                            logger.log(QString("Ошибка формата поля EX2RGB: %2").arg(ext));
                        }
                    }

                    // признак наличия третьего состояния
                    d->isThreeState = (d->foreColorExt.red() | d->foreColorExt.green() | d->foreColorExt.blue() | d->backColorExt.red() | d->backColorExt.green() | d->backColorExt.blue()) >  0;


                    // флаг рисования при отсутствии сигнала
                    d->drawOffState = query.value("DrawWhenTsOff").toBool();
                    d->description = query.value("Comment").toString().trimmed();
                    d->geometry = query.value("Geometry").toString().trimmed();

//                    d->bmpRedName = query.value("BitMapRed").toString().trimmed();
//                    d->bmpGrnName = query.value("BitMapGrn").toString().trimmed();
//                    d->bmpYelName = query.value("BitMapYel").toString().trimmed();
//                    if (d->bmpRedName.length() > 0)
//                        d->bmpRed = paResouces.GetBitmap(d.BmpRedName); //(Bitmap) rm.GetObject(d.BmpRedName));
//                    if (d->bmpGrnName.Length > 0)
//                        d->bmpGrn = paResouces.GetBitmap(d.BmpGrnName); //(Bitmap)rm.GetObject(d.BmpGrnName);
//                    if (d->bmpYelName.Length > 0)
//                        d->bmpYel = paResouces.GetBitmap(d.BmpYelName); //(Bitmap)rm.GetObject(d.BmpYelName);
//                    d->ownerDraw = d->bmpRed == null && d->bmpGrn == null && d->bmpYel == null;

                }
            }
            else
            {
                logger.log("Ошибка выполнения запроса: " + sql);
            }
        }
        else
        {
            logger.log("Ошибка открытия БД " + dbpath);
        }
    }
    catch(...)
    {
        logger.log("Исключение в функции TrnspDescription::ReadBd");
        return false;
    }

    // инициировать все статические инструменты рисования в соответстии с актуальной схемой
    //Shape.InitInstruments();
    return true;
}


// Mx,y - позиционировать в абс.координатах с учетом смещения xy
// mx,y - позиционировать относит. актуальной точки
// hx   - горизонталь от актуальной точки
// vy   - вертикаль   от актуальной точки
// lx,y - линия       от актуальной точки
// ex,y,d1,d2       эллипс, окружность
// sx,y,w,h,a1,a2 - дуга
//
bool TrnspDescription::MakePath(QPointF xyBase, QPainterPath& path, Logger* logger)
{
    bool ret = true;
    QPolygonF * poly = new QPolygonF();
    QPointF xy(xyBase);                                         // актуальная точка

    // используем регулярное выражение для проверки корректности и выделения лексем
    // не прошедшие проверку лексему не будут включены в список соответствия
    QRegularExpression re ("([MmLl] *?-?\\d+[ \\*,]+-?\\d+)|([HhVv] *?-?\\d+)|([Ee] *?-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+)|([Ss] *?-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+[ \\*,]+-?\\d+)|([Zz])");
    QRegularExpressionMatchIterator matches = re.globalMatch(geometry);

    // последовательный разбор лексем
    while (matches.hasNext())
    {
        QString lexem = matches.next().captured(0);
        QRegularExpression reg ("-?[\\d\\.]+");
        QRegularExpressionMatchIterator m = reg.globalMatch(lexem);
        QVector <float> params;
        while (m.hasNext())
        {
            params.append(m.next().captured(0).toFloat());
        }

        switch (lexem.at(0).toLatin1())
        {
            case (int)'M':                                      // Mx,y - позиционировать в абс.координатах с учетом смещения xy
                    xy = QPointF(params[0], params[1]) + xyBase;
                    * poly << xy;
                    break;
            case (int)'m':                                      // mx,y - позиционировать относит. актуальной точки
                    xy += QPointF(params[0], params[1]);
                    * poly << xy;
                    break;

            case (int)'L':
            case (int)'l':                                      // lx,y - линия       от актуальной точки
                    * poly << (xy += QPointF(params[0], params[1]));
                    break;

            case (int)'H':
            case (int)'h':                                      // hx   - горизонталь от актуальной точки
                    * poly << (xy += QPointF(params[0], 0));
                    break;

            case (int)'V':
            case (int)'v':                                      // vy   - вертикаль   от актуальной точки
                    * poly << (xy += QPointF(0, params[0]));
                    break;

            case (int)'A':
            case (int)'a':
                    break;

            case (int)'E':
            case (int)'e':                                      // ex,y,d1,d2       эллипс, окружность
                    path.addEllipse(xy = QPointF(params[0], params[1]) + xy,params[2], params[3]);
                    break;

            case (int)'S':
            case (int)'s':                                      // sx,y,w,h,startAngle,sweepLength
                    {
                        // описывающий прямоугольник опредеялется левой вершиной и размером (шириной и высотой)
                        QRectF rect(QPointF(params[0], params[1]) + xy, QSizeF(params[2], params[3]));
                        // начало отрисовки дуги - на расстоянии радиуса вправо от центра (cx+r, cy)
                        path.moveTo(xy + QPointF(params[0]+params[2], params[1] + params[3]/2));
                        // положительный угол рисуется против часовой стрелки, отрицательный - по часовой стрелке
                        path.arcTo (rect, params[4], params[5]);
                    }
                    break;

            case (int)'Z':
            case (int)'z':
                if (poly->count()!=0)
                {
                    if ((*poly)[0] != (*poly)[poly->count()-1])
                        * poly << (xy=(*poly)[0]);

                    path.addPolygon(*poly);
                }
                else
                    delete poly;
                poly = new QPolygonF();
                break;

            default:
                logger->log(QString("Ошибка синтаксиса пути: '%1', опция %2 ").arg(geometry).arg(lexem));
                break;
        }
    }
    return ret;
}


