#include <QSqlDatabase>
#include <QSqlQuery>
#include <QRectF>
#include <QtWidgets>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "shapetrnsp.h"
#include "shapeset.h"
#include "../common/boolexpression.h"

QVector<TrnspDescription *> TrnspDescription::descriptions;            // массив описателей
bool TrnspDescription::loaded = false;

ShapeTrnsp::ShapeTrnsp(QString& src, ShapeSet* parent) : DShape (src, parent)
{
//    if (!TrnspDescription::loaded)
//        TrnspDescription::readBd();

    QString s("1+100");

    BoolExpression expr(s, parent->logger(),true);
    int n = expr.GetValue();

    try
    {
        Parse(src);
    }
    catch(...)
    {
        set->logger()->log(QString("ShapeTrnsp. Исключение при разборе строки: %1").arg(src));
    }

    setDimensions ();
}

ShapeTrnsp::~ShapeTrnsp()
{

}

// Разбор строки текстового описания примитива MODE (TRNSP)
// 0   1  2     3    4      5    6
// #  Ст  id    x    y      w    h
// 20  2 50   1418  631     48   20    "!2УО&2НКП !2УО ~2УО"  1 2НСН "Смена направления"  0
// 20  2  7     71   88     40   24       "ДСН  "             2 ДСН  "Включение двойного снижения напряжения" ОДСН "Отмена двойного снижения напряжения" 0
//
// 0 - код
// 1 - станция
// 2 - ID (тип транспаранта)
// 3 - X1
// 4 - Y1
// 5 - W
// 6 - H
// Дальше все сложнее. Далее описываем выражения для разных состояний (до трех состояний)
// В общем случае так:  "Выражение1[,Мигание1] [[Выражение2] Выражение3]" [~] число ТУ [[ТУ1 "Описание ТУ1"] ТУ2 "Описание ТУ2"] ПРИЗНАК_ПРИВЯЗКИ

void ShapeTrnsp::Parse(QString& src)
{
    bool ok   = true,
         ret  = false;
    int  indx = 0;

    // заменяю кавычки на кавычки с пробелом, чтобы отделить кавычки от лексем, и делаю разбор
    QStringList lexems = src.replace("\"", " \" ").split(' ',QString::SkipEmptyParts);

    type = (ShapeType)lexems[indx++].toInt(&ret);    ok &= ret;
    idst = lexems[indx++].toInt(&ret);               ok &= ret;
    idObj= lexems[indx++].toInt(&ret);               ok &= ret;
    idObj++;                                                // приводим в соответствие нумерацию траспарантов в БД и SHAPE-файле

    x1    = lexems[indx++].toFloat(&ret);            ok &= ret;
    y1    = lexems[indx++].toFloat(&ret);            ok &= ret;
    width = lexems[indx++].toFloat(&ret);            ok &= ret;
    height= lexems[indx++].toFloat(&ret);            ok &= ret;

    if (idObj == TRNSP_BUTTON)
        height -= 3;                                        // для башиловских "кнопок" поджимаем размер на 3 пиксела

    x2 = x1 + width;
    y2 = y1 + height;

    // ищем описатель свойств и рассчитываем графику с учетом координат транспаранта
    property = idObj>=0 && idObj<TrnspDescription::descriptions.count() ? TrnspDescription::descriptions[idObj] : nullptr;
    if (property == nullptr)
        set->logger()->log(QString("ShapeTrnsp. Не найдено описание транспаранта с типом #%1: '%2'").arg(idObj).arg(src));
    else
    {
        if (property->geometry.length() > 0)
            property->MakePath(QPointF(x1,y1), path, set->logger());
        indicator = property->nameTsDefault == "Индикатор";
    }
    enableTu = false;

    if (lexems.length() < 8)                                // если вообще нет спецификации ТС - выход
        return;

    // сначала обработаем "частный случай" старого формата: всего 8 параметров, последний без кавычек)
    if (lexems[indx++] != "\"")                             // 7 ТС1
    {
/*
        StsTsExpr[0] = new LogicalExpression(ar[indx - 1], st.GetVar,null);
        if (!StsTsExpr[0].Valid)
            parent.Log(string.Format("ShapeTrnsp. Ошибка выражения в описании транспаранта '{0}': {1}. FILE {2}. LINE {3}: '{4}'", StsTsExpr[0].Source, StsTsExpr[0].ErrorText, Parent.SrcFile, Parent.LineNumber, str));
*/
        return;
    }
/*
                    string token;
                    // имеем актуальную лексему с кавычками: " Выражение1[,Мигание1] [[Выражение2] Выражение3] "
                    // ПРОБЛЕМА: Может быть и такое описание: " " - пробел внутри кавычек, надо уметь обработать
                    int i = 0;
                    while ((token = ar[indx++]) != "\"")
                    {
                        if (i >= 3)
                        {
                            parent.Log(string.Format("ShapeTrnsp. Обнаружено более трех выражений в поле ТС: '{0}'", str));
                            return;
                        }
                        // ищем мигающее вражение
                        string[] sPulse = token.Split(new char[] {',',});
                        if (sPulse.Length > 1)
                        {
                            StsTsExpr   [i] = new LogicalExpression(sPulse[0], st.GetVar, null);
                            StsPulseExpr[i] = new LogicalExpression(sPulse[1], st.GetVar, null);
                            if (!StsTsExpr[i].Valid)
                                parent.Log(string.Format("ShapeTrnsp. Ошибка выражения в описании транспаранта '{0}': {1}. FILE {2}. LINE {3}: '{4}'", StsTsExpr[i].Source, StsTsExpr[i].ErrorText, Parent.SrcFile, Parent.LineNumber, str));
                            if (!StsPulseExpr[i].Valid)
                                parent.Log(string.Format("ShapeTrnsp. Ошибка выражения в описании транспаранта '{0}': {1}. FILE {2}. LINE {3}: '{4}'", StsPulseExpr[i].Source, StsPulseExpr[i].ErrorText, Parent.SrcFile, Parent.LineNumber, str));
                        }
                        else
                        {
                            StsTsExpr[i] = new LogicalExpression(token, st.GetVar, null);
                            if (!StsTsExpr[i].Valid)
                                parent.Log(string.Format("ShapeTrnsp. Ошибка выражения в описании транспаранта '{0}': {1}. FILE {2}. LINE {3}: '{4}'", StsTsExpr[i].Source, StsTsExpr[i].ErrorText, Parent.SrcFile, Parent.LineNumber, str));
                        }
                        i++;
                    }

                    if (indx > ar.Length - 1)
                        return;

                    // [~] число ТУ [[ТУ1 "Описание ТУ1"] ТУ2 "Описание ТУ2"] ПРИЗНАК_ПРИВЯЗКИ
                    //
                    token = ar[indx++];
                    if (token == "~")
                    {
                        Pulsing = true;
                        if (indx > ar.Length - 1)
                            return; // нет ТУ
                        token = ar[indx++];
                    }

                    // число ТУ [[ТУ1 " Описание ТУ1 "] ТУ2 " Описание ТУ2 "] ПРИЗНАК_ПРИВЯЗКИ
                    int ntu = Convert.ToInt32(token);
                    if (ntu > 2)
                    {
                        parent.Log(
                            string.Format(
                                "Конструктор ShapeTrnsp. Некорректное число команд ТУ в описании примитива: {0}", ntu));
                        ntu = 2;
                    }

                    for (i = 0; i < ntu; i++)
                    {
                        tuNames[i] = ar[indx++];
                        indx++;

                        while ((token = ar[indx++]) != "\"")
                        {
                            tuTexts[i] += token + " ";
                        }
                        EnableTu = true;
                    }


                    // далее может идти признак привязки транспаранта к следующему текстовому примитиву
                    if (indx > ar.Length - 1)
                        return; // закончился примитив

                    bool bLink = Convert.ToInt32(ar[indx++]) != 0 ? true : false;
                    if (indx > ar.Length - 1)
                        return; // закончился примитив

                    token = ar[indx++];
                    // далее может идти либо комментарий, либо описание транспаранта в кавычках
                    if (token == "\"")
                    {
                        // нашли кавычку, значит есть индивидуальное описание примитива
                        ToolTipText = "";
                        while ((ar[indx] != "\"") && indx < ar.Length - 1)
                            ToolTipText += ar[indx++] + " ";
                        if (indx < ar.Length)
                            indx++;
                        else
                        {
                            parent.Log(string.Format("ShapeTrnsp. Нет завершающих кавычек в описании примитива '{0}'", str));
                        }
                    }
                    if (indx > ar.Length - 1)
                        return; // закончился примитив

                    // ищем комментарий
                    Comment = "";
                    if (ar[indx].IndexOf(';') == 0)
                    {
                        while (indx <= ar.Length - 1)
                            Comment += ar[indx++] + " ";
                    }

*/
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
            painter->drawPath(path);
        }
        else
        {
            painter->drawRect(rect);
        }

        QTextOption op(Qt::AlignCenter);
        painter->drawText(rect, property->name, op);
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
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "qsqlite");
        dbSql.setDatabaseName(dbpath);
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

                    d->foreColorOn  = QColor::fromRgb(query.value("On_ClrR").toInt(), query.value("On_ClrG").toInt(), query.value("On_ClrB").toInt());
                    d->backColorOn  = QColor::fromRgb(query.value("On_BckR").toInt(), query.value("On_BckG").toInt(), query.value("On_BckB").toInt());

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
                            QStringList bck = rgbs[1].split(' ', QString::SkipEmptyParts);
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

                    if (d->geometry.length() > 0)
                    {
                        int a=99;
                    }

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

    if (geometry.indexOf("e") >= 0)
        int a=99;

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


