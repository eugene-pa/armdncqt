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
#include "../spr/peregon.h"
#include "shapetrain.h"

std::unordered_map<int, TrnspDescription *> TrnspDescription::descriptions;// массив описателей
bool TrnspDescription::loaded = false;

QBrush ShapeTrnsp::brushUndefined;                          // общая кисть неопределенного состояния
QPen   ShapeTrnsp::penUndefined;                            // общее перо  неопределенного состояния
QPen   ShapeTrnsp::whitePen;                                // общее белое перо

ShapeTrnsp::ShapeTrnsp(QString& src, ShapeSet* parent) : DShape (src, parent)
{
    pulsing = false;
    noprg = 0;
    prg = nullptr;
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
    roundedTuRect = QRectF(XY + QPointF(-3, -3), QSizeF(width+6, height + 6));

    // ищем описатель свойств и рассчитываем графику с учетом координат транспаранта
    prop = idObj>=0 && idObj<(int)TrnspDescription::descriptions.size() ? TrnspDescription::descriptions[idObj] : nullptr;
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

    // имеем актуальную лексему с кавычками: " Выражение1[,Мигание1] [[Выражение2] Выражение3] "
    // ПРОБЛЕМА: Может быть и такое описание: " " - пробел внутри кавычек, надо уметь обработать
    int index = 8;
    int i = 0;
    QString token;
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
        i++;
    }

    // обраюотаем направление и номер перегона на транспарнтах слепых перегонов
    if (prop->id==TRNSP_BLIND_L || prop->id==TRNSP_BLIND_R)
    {
        dir = 0;
        QString s = stsExpr[0]->Source();
        if (s.indexOf("Ч") ==0)
            dir = 1;
        if (s.indexOf("Н") ==0 || s.indexOf("H") ==0)
            dir = -1;
        QRegularExpression regexEx("\\A[ЧН]\\[#\\d+\\]");
        QRegularExpressionMatch match = regexEx.match(s);
        if (match.hasMatch())
        {
            QRegularExpression regexExN("\\d");
            QRegularExpressionMatch match = regexExN.match(s);
            if (match.hasMatch())
            {
                noprg = match.captured().toInt();
                prg = Peregon::GetById(noprg);
            }
        }
        else
            log (QString("ShapeTrnsp. Ошибка выражения в описании транспаранта 'Поезда на перегоне': %1").arg(src));
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
    //bool bLink = lexems[index++].toInt(&ret) != 0 ? true : false;
    index++;                                                // игнорирую привязку
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


    if (!toolTipText.length() && prop!=nullptr && prop->description.length())
        toolTipText = prop->description;
}


// инициализация статических инструментов отрисовки
void ShapeTrnsp::InitInstruments()
{
    brushUndefined = QBrush(colorScheme->GetColor("Undefined"   ));       // неопред.состояние
    penUndefined   = QPen  (colorScheme->GetColor("Undefined"   ));
    whitePen       = QPen  (Qt::white,2);
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
    QString s = "Ст." + StationName();
    if (toolTipText.length())
        s += "\r\n" + toolTipText;
    else
    if (prop->description.length())
        s += "\r\n" + prop->description;

    // ТС
    for (int i=1; i<=3; i++)
    {
        if (stsExpr[i-1] != nullptr && stsExpr[i-1]->Source().length())
        {
            s += "\r\nТС" + QString::number(i) + ": " + stsExpr[i-1]->Source();
            if (stsPulseExpr[i-1]!=nullptr && stsPulseExpr[i-1]->Source().length())
                s += ",   " + stsPulseExpr[i-1]->Source();
            s += " = " + QString::number(stsExpr[i-1]->ValueBool());
        }
    }

    if (tuNames.length() > 0)
        s += "\r\nТУ ВКЛ: " + tuNames[0];
    if (tuNames.length() > 1)
        s += "\r\nТУ ВЫКЛ: " + tuNames[1];


    return s;
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
    state->set(Status::StsUndefined, false);
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

        blinking = false;

        state->set(StsOn , stsExpr[0] == nullptr ? false : stsExpr[0]->ValueBool());

        // если первое состояние == false, проверяем второе
        if (!(*state)[StsOn])
        {
            state->set(StsOff , stsExpr[1] == nullptr ? true : stsExpr[1]->ValueBool());
            if (!(*state)[StsOff])
                state->set(StsExt , stsExpr[2] == nullptr ? true : stsExpr[2]->ValueBool());
        }

        // отдельно обрабатываем транспарант режимов управления
        if (idObj==TRNSP_DU && st != nullptr)
        {
            state->set(StsOn , st->IsDu());                 // состояние 1 - ДУ
            state->set(StsOff, st->IsRu());                 // состояние 0 - РУ
            state->set(StsExt, st->IsAu() || st->IsSu());   // состояние 2 - АУ или СУ
        }

        // "Особый" транспарант TRNSP_TEXT=25 не имеет описания активного состояния, но в старой версии отображается в пассивном состоянии
        // обеспечиваем совместимость, устанавливая StsOff = true
        if (idObj==TRNSP_TEXT && st != nullptr && !(*state)[StsOn])
           state->set(StsOff, true);

        // реализация мигания с помощью расширения выражения через запятую, например: АДН,~АДН
        // ВАЖНО: мигает то соcтояние транспаранта, в котором обнаружено выполнение выражения мигания
        for (int i = 2; i >= 0; i--)
        {
            if (stsPulseExpr[i] != nullptr && stsPulseExpr[i]->ValueBool())
            {
                blinking = true;
                state->set((i == 0 ? StsOn : i == 1 ? StsOff : StsExt),true);
            }
        }

        // Если транспарант должен пульсировать и StsAct[StsOn] - обеспечиваем пульсацию сменой состояния ON/OFF
        if (pulsing && (*state)[StsOn] )
            blinking = true;                                // Мигать будет активное состояние на чистом фоне
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
    accept();

    if (blinking && DShape::globalPulse)
        return;

    // отрисовка если транспарант MUST DRAW всегда или есть активные состояния
    if (prop->drawOffState || (*state)[StsOn] || (*state)[StsOff] || (*state)[StsExt])
    {
//        bool on = (*state)[StsOn],
//             off = (*state)[StsOff],
//             ext = (*state)[StsExt],
//             expire = (*state)[Status::StsExpire],
//             undefined = (*state)[Status::StsUndefined];
        painter->setRenderHint(QPainter::Antialiasing);

        Palitra& palitra =  (*state)[Status::StsExpire]           ? prop->palitras[4] :
                            (*state)[Status::StsUndefined]        ? prop->palitras[3] :
                            (*state)[StsOn]                       ? prop->palitras[0] :     // ON
                            (*state)[StsOff]                      ? prop->palitras[1] :     // OFF
                            (*state)[StsExt] && prop->isThreeState? prop->palitras[2] :     // EXT
                                                                    prop->palitras[4];

        // отдельно обрабатываем транспарант режимов управления
        if (idObj==TRNSP_DU && st != nullptr && st->IsSu())
        {
            palitra = prop->palitras[3];                                                    // EXT2
        }

        // описание fore и back для индикаторов в БД сделано через ж., можно поправить, чтобы избежать лишних проверок ниже
        // для индикатора в качестве основной кисти заливки эллипса используется основной цвет,
        // для прямоугольников основной цвет - надпись. а кисть - фон
        QBrush * brush  = indicator ? &palitra.brushFore : &palitra.brushBack;
        QPen   * pen    = indicator ? &palitra.penBack   : &palitra.penFore;
        painter->setPen  (pen  ==nullptr ? Qt::NoPen   : *pen);
        painter->setBrush(brush==nullptr ? Qt::NoBrush : *brush);

        if (indicator)                                      // индикатор
        {
            painter->drawEllipse(rect);
            if (enableTu)
            {
                painter->setBrush(Qt::NoBrush);
                painter->setPen  (whitePen);
                painter->drawEllipse(rect.center(), rect.width()/2 + 3, rect.height()/2 + 3);
            }
        }
        else
        {
            QTextOption op(Qt::AlignCenter);

            if (path.length() != 0)                         // геометрия пути
            {
                painter->drawPath(path);                    // некоторые транспаранты могут иметь геометрию + текст, например ЧКЖ/НКЖ
                                                            // можно опционировать в поле БД, можно зашить в код
                if (idObj==TRNSP_CHKZH || idObj==TRNSP_NKZH)
                {
                    if (!palitra.suited)
                        suiteFont (painter, palitra);
                    QRectF r = rect;
                    if (idObj==TRNSP_NKZH)
                        r.moveTopLeft (QPointF(rect.x() - 8, rect.y()-8));
                    else
                        r.moveTopLeft (QPointF(rect.x() + 8, rect.y()-8));
                    painter->drawText(r, palitra.text, op);
                }
            }
            else                                            // поезда на слепых перегонах отрисовываем отдельно
            if (prop->id==TRNSP_BLIND_L || prop->id==TRNSP_BLIND_R)
            {
                if (prg != nullptr)
                {
                    // нужно уметь отображать несколько поездов со сдвигом
                    if (isEvn() && prg->evnTrains.size())
                    {
                        for (uint i=0; i<prg->evnTrains.size(); i++)
                        {
                            float dx = (prg->leftOddOrient ? 10 : -10)*(int)i;
                            float dy = (prg->leftOddOrient ? -18 : 18)*(int)i;
                            ShapeTrain::drawTrain(painter, prg->evnTrains[i], prg->leftOddOrient, xy() + QPointF(dx,dy+24), x2y2() + QPointF(dx,dy+24));
                        }
                    }
                    else
                    // нужно уметь отображать несколько поездов со сдвигом
                    if (isOdd() && prg->oddTrains.size())
                    {
                        for (uint i=0; i<prg->oddTrains.size(); i++)
                        {
                            float dx = (prg->leftOddOrient ? -10 : 10)*(int)i;
                            float dy = (prg->leftOddOrient ? 18 : -18)*(int)i;
                            ShapeTrain::drawTrain(painter, prg->oddTrains[i], prg->leftOddOrient , xy() + QPointF(dx,dy+24), x2y2() + QPointF(dx,dy));
                        }
                    }
                }
            }
            else                                            // окантовка с текстом
            {
                painter->drawRect(rect);

                painter->setFont(palitra.font);

                // особо обрабатываю транспарант 25 - TRNSP_TEXT, выводим имя сигнала вместо названия транспаранта
                if (prop->id==TRNSP_TEXT || palitra.text.length()==0)
                {
                    painter->drawText(rect, stsExpr[0]->Source(), op);
                }
                else
                {
                    if (!palitra.suited)
                        suiteFont (painter, palitra);
                    painter->drawText(rect, palitra.text, op);
                }
            }

            if (enableTu)
            {
                painter->setBrush(Qt::NoBrush);
                painter->setPen  (whitePen);
                painter->drawRect(roundedTuRect);
            }
        }
    }

}

// подгонка размера шрифта под размер обрамляющего прямоугольника
void ShapeTrnsp::suiteFont (QPainter * painter, Palitra& palitra)
{
    QTextOption option(Qt::AlignCenter);
    QRectF boundRect;
    for (int i = palitra.font.pointSize(); i>6; i--)
    {
        palitra.font.setPointSize(i);
        painter->setFont(palitra.font);
        QRectF boundRect = painter->boundingRect(rect, palitra.text, option);
        if (rect.width() - boundRect.width() >= 6)
            break;
        // уменьшаем не только размер, но и вес шрифта (до нормы = 50)
        if (palitra.font.weight() > 55)
            palitra.font.setWeight(palitra.font.weight() - 5);
    }
    palitra.suited = true;
}

