#ifndef TRNSPDESCRIPTION_H
#define TRNSPDESCRIPTION_H

#include "../common/defines.h"
#include "../common/logger.h"
#include "palitra.h"

enum
{
    maxStates = 4,
};



//  класс описателя типа транспаранта; формируется путем чтения таблицы транспарантов из БД
class TrnspDescription
{
public:
static std::unordered_map<int, TrnspDescription *> descriptions;            // массив описателей
static bool loaded;
    TrnspDescription(QSqlQuery& query, Logger& logger);
    ~TrnspDescription();
static bool readBd (QString& dbpath, Logger& logger);
    bool MakePath(QPointF xyBase, QPainterPath& path, Logger * logger);
//private:
    int         id;
    QString		nameTsDefault;								// имя определяющего сигнала ТС
    Palitra     palitras[maxStates+1];

    bool        isThreeState;                               // признак наличия третьего состояния
    bool		drawOffState;								// флаг прорисовки пассивного состояния транспаранта; сейчас игнорируем и прорисовываем все
    int         width;										// ширина
    int 		height;										// высота

    QString     geometry;                                   // описание геометрии примитива
    QString     description;                                // описание (тултип запоминаем)

//    QString     NameIcon;                                   // имя иконки
//  работаю только с векторной графикой
//    QBitmap*	pBmpRed;									// битовая карта пасс.сост.
//    QBitmap*	pBmpYel;									// битовая карта акт. сост.
//    QBitmap*	pBmpGrn;									// битовая карта акт. сост.
//    void ReplaceSysColors();
//    QBitmap * MakeBitmapCompatible (QPainter *, QBitmap *);
    //bool ParsePathStr (QPointF xy, QString& geometry, QString errors);
};

#endif // TRNSPDESCRIPTION_H
