#ifndef SHAPETRNSP_H
#define SHAPETRNSP_H

#include "shape.h"
#include "../common/logger.h"
#include "palitra.h"
#include "trnspdescription.h"


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

    bool isEvn() { return dir ==  1; }
    bool isOdd() { return dir == -1; }
    int  dir;                                               // направление на транспаранте перегона -1 - неч, +1 чет
    int  noprg;                                             // номер перегона
    class Peregon * prg;                                    // справочник перегона

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
    virtual void  Draw (QPainter*) override;                // функция рисования
    virtual void  Parse(QString&) override;                 // разбор строки описания
    virtual bool  CheckIt() override;
    virtual void  FixUpUnsafe() override;
    virtual QString Dump() override;
    virtual QString ObjectInfo() override;
    virtual void  Prepare() override;
    virtual void accept() override;                         // вычисление состояния примитива
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void suiteFont (QPainter *, Palitra& palitra);          // подгонка размера шрифта под занимаемое место
};

#endif // SHAPETRNSP_H
