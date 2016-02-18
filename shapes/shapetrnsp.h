#ifndef SHAPETRNSP_H
#define SHAPETRNSP_H

#include "shape.h"
#include "../common/logger.h"


// класс описателя отдельного состояния транспаранта
// (для состояний ON, OFF, EXT, EXT2)
class Palitra
{
public:
    Palitra()
    {
        QString family = "Segoe UI";
#ifdef Q_OS_WIN
        int size   = 11;
        int weight = 65;
#endif
#ifdef Q_OS_MAC
        family = "Segoe UI";
        int size   = 16;
        int weight = 50;
#endif
#ifdef Q_OS_LINUX
        int size   = 12;
        int weight = 50;
#endif
        font = QFont(family, size, weight);

        colorFore = colorBack = Qt::darkGray;              // по умолчанию - серая палитра
        brushFore = brushBack = Qt::lightGray;
        penFore   = penBack   = QPen(Qt::gray);
        suited = false;
        valid  = false;
    }

    QFont   font;                                           // шрифт
    QColor  colorFore;                                      // цвет переднего плана
    QColor  colorBack;                                      // цвет фона
    QBrush  brushFore;                                      // кисть переднего плана
    QBrush  brushBack;                                      // кисть фона
    QPen    penFore;                                        // перо переднего плана
    QPen    penBack;                                        // перо фона
    QString text;                                           // текст
    bool    valid;
    bool    suited;                                         // размер подогнан!

    // инициализация
    void init (QColor fore, QColor back)
    {
        valid = fore.red() | fore.green() | fore.blue() | back.red() | back.green() | back.blue();
        colorFore = fore;
        colorBack = back;
        brushFore = QBrush(fore);
        brushBack = QBrush(back);
        penFore   = QPen(fore);
        penBack   = QPen(back);
    }
};

enum
{
    maxStates = 4,
};


//  класс описателя типа транспаранта; формируется путем чтения таблицы транспарантов из БД
class TrnspDescription
{
public:
//static QVector<TrnspDescription *> descriptions;            // массив описателей
static QHash<int, TrnspDescription *> descriptions;            // массив описателей
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


class ShapeTrnsp : public DShape
{
protected:
    enum
    {
        StsOff = 2,                                         // пассивное состояние
        StsOn ,                                             // активность состояния 1
        StsExt,                                             // активность состояния 2
    };

    static QBrush brushUndefined;
    static QPen   penUndefined;
    static QPen   whitePen;

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

    QPoint XY_titleOn;                                      // X1, Y1 для текста в состоянии ON
    QPoint XY_titleOff;                                     // X1, Y1 для текста в состоянии OFF
    QPoint XY_titleExt;                                     // X1, Y1 для текста в состоянии EXT

    QRectF roundedTuRect;                                   // окантовка

    BoolExpression* stsExpr     [maxStates];                // формулы для определения активности 3-х состояний
    BoolExpression* stsPulseExpr[maxStates];                // формулы для определения мигания    3-х состояний
    QStringList tuNames;                                    // имена команд ТУ
    QStringList tuTexts;                                    // расшифровка ТУ

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

        TRNSP_TEXT        = 25,                                 // был TRNSP_SAUT - особый транспарант: выводим имя сигнала вместо имени транспаранта

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

public:
    ShapeTrnsp(QString& src, ShapeSet* parent);
    ~ShapeTrnsp();
static void InitInstruments();                              // инициализация статических инструментов отрисовки

protected:
    virtual void  Draw (QPainter*);                         // функция рисования
    virtual void  Parse(QString&);                          // разбор строки описания
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void  Prepare();
    virtual void accept();                                  // вычисление состояния примитива
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

    void suiteFont (QPainter *, Palitra& palitra);          // подгонка размера шрифта под занимаемое место
};

#endif // SHAPETRNSP_H
