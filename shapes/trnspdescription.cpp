#include "trnspdescription.h"
#include "../spr/sprbase.h"


// конструктор описаиеля транспаранта; добавляет указатель на класс в вектор описателей, при необходимости расширяя его размерность
TrnspDescription::TrnspDescription(QSqlQuery& query, Logger& logger)
{
    bool ret;
    id = query.value("No").toInt(&ret);

    // Даю возможность задать разные надписи для разных состояний транспаранта, например: ДУ;РУ;АУ;СУ
    // Для этого в поле Text должны быть записаны лексемы в порядке следования состояний; всего 4 состояния
    QString s = query.value("Text").toString().trimmed();
    QStringList names = s.split(';', QString::SkipEmptyParts);
    // по умолчанию имена совпадают
    if (names.length() > 0)
        for (int i=0; i<maxStates+1; i++)
            palitras[i].text = names[0];
    for (int i=1; i<names.length(); i++)
        palitras[i].text = names[i];


    nameTsDefault = query.value("Ts").toString().trimmed();
    width  = query.value("W").toInt(&ret);
    height = query.value("H").toInt(&ret);

    // создаем перья и кисти для разных состояний транспаранта (ОN/OFF/EXT/EXT2)
    // ОN
    palitras[0].init(QColor::fromRgb(query.value("On_ClrR").toInt(), query.value("On_ClrG").toInt(), query.value("On_ClrB").toInt()),
                     QColor::fromRgb(query.value("On_BckR").toInt(), query.value("On_BckG").toInt(), query.value("On_BckB").toInt()));
    // OFF
    palitras[1].init(QColor::fromRgb(query.value("Off_ClrR").toInt(), query.value("Off_ClrG").toInt(), query.value("Off_ClrB").toInt()),
                     QColor::fromRgb(query.value("Off_BckR").toInt(), query.value("Off_BckG").toInt(), query.value("Off_BckB").toInt()));
    // EXT
    palitras[2].init(QColor::fromRgb(query.value("Ex_ClrR").toInt(), query.value("Ex_ClrG").toInt(), query.value("Ex_ClrB").toInt()),
                     QColor::fromRgb(query.value("Ex_BckR").toInt(), query.value("Ex_BckG").toInt(), query.value("Ex_BckB").toInt()));
    // EXT2
    palitras[3].init(QColor::fromRgb(query.value("Ex_ClrR").toInt(), query.value("Ex_ClrG").toInt(), query.value("Ex_ClrB").toInt()),
                     QColor::fromRgb(query.value("Ex_BckR").toInt(), query.value("Ex_BckG").toInt(), query.value("Ex_BckB").toInt()));

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
                palitras[3].init(QColor::fromRgb(clr[0].toInt(), clr[1].toInt(), clr[2].toInt()),
                                    QColor::fromRgb(bck[0].toInt(), bck[1].toInt(), bck[2].toInt()));
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
    isThreeState = palitras[2].valid;

    // флаг рисования при отсутствии сигнала
    drawOffState = query.value("DrawWhenTsOff").toBool();
    description = query.value("Comment").toString().trimmed();
    if (description.length())
        int a = 99;
    geometry = query.value("Geometry").toString().trimmed();

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
                    new TrnspDescription(query, logger);
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
        std::vector <float> params;
        while (m.hasNext())
        {
            params.push_back(m.next().captured(0).toFloat());
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



