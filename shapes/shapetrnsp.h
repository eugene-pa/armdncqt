#ifndef SHAPETRNSP_H
#define SHAPETRNSP_H

#include "shape.h"
#include "../common/logger.h"

//  класс описателя типа транспаранта; формируется путем чтения таблицы транспарантов из БД
class TrnspDescription
{
public:
static QVector<TrnspDescription *> descriptions;            // массив описателей
static bool loaded;
    TrnspDescription(int _id);
    ~TrnspDescription();
static bool readBd (QString& dbpath, Logger& logger);
    bool MakePath(QPointF xyBase, QPainterPath& path, Logger * logger);
//private:
    int         id;
    QString		name;										// имя транспаранта из БД
    QString		name2;										// имя транспаранта из БД альтернативное (акт.состояния)
    QString		name3;										// имя транспаранта из БД альтернативное (третьего состояния)
    QString		name4;										// имя транспаранта из БД четвертого состояния

    QString		nameTsDefault;								// имя определяющего сигнала ТС
    bool		ownerDraw;   								// признак программной отрисовки

//	bool		Pulse;										// признак использования мигающего сигнала
    QColor      foreColorOn;								// Цвет символов в состоянии ON
    QColor      backColorOn;								// Цвет фона     в состоянии ON
    QColor      foreColorOff;								// Цвет символов в состоянии OFF
    QColor      backColorOff;								// Цвет фона     в состоянии OFF
    QColor      foreColorExt;								// Цвет символов третьего состояния или null
    QColor      backColorExt;								// Цвет фона     третьего состояния или null
    QColor      foreColorExt2;								// Цвет символов 4-го состояния или null
    QColor      backColorExt2;								// Цвет фона     4-го состояния или null
    bool        isThreeState;                               // признак наличия третьего состояния

    bool		drawOffState;								// флаг рисования при отсутствии сигнала
    int         width;										// ширина
    int 		height;										// высота

    QString     geometry;                                   // описание геометрии примитива
    //QPainterPath path;                                      // сформированный путь (коллекция примитивов)
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


class ShapeTrnsp : public DShape
{
protected:
    TrnspDescription * prop;                                // указатель на описатель типа транспаранта

    QString	    helperOn ;                                  // имя меню включить
    QString     helperOff;                                  // имя меню отключить
    QString     description;                                // Описание
    QString     toolTipText;                                //

    bool        oneNameExpression;                          // логич. выражение - имя сигнала
    bool        otu;                                        // транспарант для ввода ОТУ
    QString     comment;                                    //
    bool        pulsing;									// пульсирует при истине в основном выражениии (тильда после описания ТС)
    int         norc;                                       // номер РЦ (поле на перспективу)
    bool        enableTu;                                   // признак возможности ОТУ
    bool        indicator;                                  // признак индикатора-лампочки ( имя ТС в БД == "Индикатор")
    QPainterPath path;                                      // сформированный путь (коллекция примитивов)

    enum
    {
        maxStates = 3,
    };
    BoolExpression* stsExpr     [maxStates];                // формулы для определения активности 3-х состояний
    BoolExpression* stsPulseExpr[maxStates];                // формулы для определения мигания    3-х состояний
    QStringList tuNames;                                    // имена команд ТУ
    QStringList tuTexts;                                    // расшифровка ТУ
public:
    enum TrnspTypes
    {
        TRNSP_A           = 1,
        TRNSP_APN         = 2,
        TRNSP_APCH        = 3,
        TRNSP_TU          = 4,
        TRNSP_ADN         = 5,
        TRNSP_ADCH        = 6,
        TRNSP_DAYNIGHT    = 7,
        TRNSP_DSN         = 8,
        TRNSP_AUM         = 9,
        TRNSP_AD          = 10,
        TRNSP_DU          = 11,
        TRNSP_RON         = 12,
        TRNSP_ROCH        = 13,
        TRNSP_BLIND_L     = 14,                                 // слепой перегон влево
        TRNSP_BLIND_R     = 15,                                 // слепой перегон вправо
        TRNSP_AUM_L       = 16,
        TRNSP_AUM_R       = 17,
        TRNSP_PONAB       = 18,
        TRNSP_F1          = 19,
        TRNSP_F2          = 20,
        TRNSP_F           = 21,
        TRNSP_TS          = 22,
        TRNSP_UKSPS       = 23,
        TRNSP_TRANSMIT    = 24,
        TRNSP_SAUT        = 25,
        TRNSP_RF1         = 26,
        TRNSP_RF2         = 27,
        TRNSP_MAIN_RSRV   = 28,
        TRNSP_KP          = 29,
        TRNSP_DISK_A      = 30,
        TRNSP_DISK_T      = 31,
        TRNSP_CHKZ        = 32,                                 // ЧКЗ
        TRNSP_CHKZH       = 33,                                 // ЧКЖ
        TRNSP_NKZ         = 34,                                 // НКЗ
        TRNSP_NKZH        = 35,                                 // НКЖ
        TRNSP_BUILDING    = 36,
        TRNSP_SBF         = 37,
        TRNSP_WHT         = 38,
        TRNSP_KPP         = 39,
        TRNSP_IR          = 40,
        TRNSP_VZ          = 41,
        TRNSP_ZS          = 42,
        TRNSP_NGB         = 43,
        TRNSP_SA          = 44,
        TRNSP_CHDK        = 45,
        TRNSP_UPOR_ODD    = 46,
        TRNSP_UPOR_EVN    = 47,
        TRNSP_RED         = 48,
        TRNSP_GRN         = 49,
        TRNSP_YEL         = 50,
        TRNSP_PRG_L       = 51,                                 // занятость перегона налево
        TRNSP_PRG_R       = 52,                                 // занятость перегона направо
        TRNSP_REPAIR      = 53,
        TRNSP_TUNNEL_L    = 54,
        TRNSP_TUNNEL_R    = 55,
        TRNSP_KTSM1       = 56,
        TRNSP_KTSM2       = 57,
        TRNSP_GRNWGT      = 58,
        TRNSP_YELWHT      = 59,
        TRNSP_YELGRN      = 60,
        TRNSP_PRG_L_SMALL = 61,
        TRNSP_PRG_R_SMALL = 62,
        TRNSP_BUILDING_SAMLL= 63,
        TRNSP_BUTTON      = 64,
    };

    ShapeTrnsp(QString& src, ShapeSet* parent);
    ~ShapeTrnsp();

    virtual void  Draw (QPainter*);                         // функция рисования
    virtual void  Parse(QString&);                          // разбор строки описания
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void  Prepare();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

    inline QColor color1() { return prop->foreColorOn;   }
    inline QColor color2() { return prop->foreColorOff;  }
    inline QColor color3() { return prop->foreColorExt;  }
    inline QColor color4() { return prop->foreColorExt2; }
    inline QColor back1 () { return prop->backColorOn;   }
    inline QColor back2 () { return prop->backColorOff;  }
    inline QColor back3 () { return prop->backColorExt;  }
    inline QColor back4 () { return prop->backColorExt2; }
};

#endif // SHAPETRNSP_H
