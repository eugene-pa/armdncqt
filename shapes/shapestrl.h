#ifndef SHAPESTRL_H
#define SHAPESTRL_H

#include "shape.h"

struct StrlShapeProp                                        // полное описание координат стрелки
{
    TA	align;                                              // выравнивание надписи
    int xt,   yt,                                           // позиция надписи

        x1P,  y1P,                                          // начало тонкой линии плюсового положения по прямому ходу
        x2P,  y2P,                                          // конец  тонкой линии плюсового положения по прямому ходу
        x1O,  y1O,                                          // начало тонкой линии плюсового положения на ответвление
        x2O,  y2O,                                          // конец  тонкой линии плюсового положения на ответвление

        x1,   y1,                                           // конец общего отрезка
        x0,   y0,                                           // базовая точка ветвления стрелки точка
        x2,   y2,                                           // конец рлюсового положения
        x3,   y3;                                           // конец минусового положения
};

class ShapeStrl : public DShape
{
public:
    enum StrlTypes                                          // типы стрелок
    {
                                                            // горизонтали вбок
        WEtoNE      = 0,                                    // горизонталь запад > восток ответвление на северо восток
        EWtoNW      = 1,                                    // горизонталь восток > запад ответвление на северо запад
        WEtoSE      = 2,                                    // горизонталь запад > восток ответвление на юго    восток
        EWtoSW      = 3,                                    // горизонталь восток > запад ответвление на юго    запад

                                                            // наклонные вправо / влево
        SWNEtoE     = 4,                                    // наклонная юго-запад  >  северо восток ответвление на восток
        SENWtoW     = 5,                                    // наклонная юго-восток >  северо западк ответвление на запад
        NWSEtoE     = 6,                                    // наклонная северо-запад  >  юго-восток ответвление на восток
        NESWtoW     = 7,                                    // наклонная северо-восток >  юго-запад  ответвление на западк

                                                            // вертикали вбок
        SNtoNE      = 8 ,                                   // вертикаль юг  >  север ответвление на северо восток
        SNtoNW      = 9 ,                                   // вертикаль юг  >  север ответвление на северо запад
        NStoSE      = 10,                                   // вертикаль север > юг ответвление на юго восток
        NStoSW      = 11,                                   // вертикаль север > юг ответвление на юго запад

                                                            // разветвления
        WtoNEorSE   = 12,                                   // разветвление вправо с запада  на северо восток и юго восток
        EtoNWorSW   = 13,                                   // разветвление влево  с востока на северо запад и юго запад

                                                            // наклонные вверх / вниз
        SWNEtoN     = 14,                                   // наклонная юго-запад > северо-восток ответвление на север
        SENWtoN     = 15,                                   // наклонная юго-восток > северо-запад ответвление на север
        NWSEtoS     = 16,                                   // наклонная северо-запад > юго-восток ответвление на юг
        NESWtoS     = 17,                                   // наклонная северо-восток > юго-запад ответвление на юг

        nMaxStrlType = 18,                                  // число типов стрелок
    };

    enum NormalPositions                                    // направление в положении ПЛЮС
    {
        Direct      = 0,                                    // прямо
        Sideways    = 1,                                    // на ответвление
    };

    enum                                                    // состояние визуализации
    {
        StsPlus     = 2,                                    // +
        StsMinus    ,                                       // -
        StsAlarm    ,                                       // взрез индивидуальный
        StsZmk      ,                                       // замыкание
        StsIzs      ,                                       // искусственное замыкание, блокировка
        StsOtu      ,                                       // стрелка выбрана для ОТУ вспомогат.перевода
//      StsBlock    ,                                       // блокирована
        StsMu       ,                                       // МУ
        StsBusy     ,                                       // занятость
        StsRqRoute  ,                                       // в устанавливаемом маршруте
        StsPzdRoute ,                                       // в поездном маршруте
        StsMnvRoute ,                                       // в маневровом маршруте
        StsPassed   ,                                       // пройдена в маршруте
        StsIr       ,                                       // искусств.разделка
    };
protected:

    static StrlShapeProp strlProp60[nMaxStrlType];          // геометрия стрелок 60 градусов
    static StrlShapeProp strlProp45[nMaxStrlType];          // геометрия стрелок45 градусов

    static QBrush * BrushNormal;                            // кисть для отрисовки номера стрелки в норм.состоянии

    static QPen * PenFree;                                  // свободное состоняние
    static QPen * PenBusy;                                  // занятая стрелка
    static QPen * PenOtuRect;                               // желтое перо окантовки (ОТУ)
    static QPen * PenRqRoute;                               // в устанавливаемом маршруте
    static QPen * PenPzdRoute;                              // в поездном маршруте
    static QPen * PenMnvRoute;                              // в маневровом маршруте
    static QPen * PenZmk;                                   // замкнутая РЦ не в неиспользованном маршруте
    static QPen * PenIr;                                    // искусственная разделка (мигает поверх других состояний)
    static QPen * PenMuRect;                                // окантовка стрелки на МУ
    static QPen * PenUndefined;                             // объект неопределен - пассивная отрисовка
    static QPen * PenNormalOk;                              // тонкая линия нормали, корректное состояние
    static QPen * PenNormalAlarm;                           // тонкая линия нормали, взрез
    static QPen * PenAlarmPulse1;                           // авария (1-я фаза мигания)
    static QPen * PenAlarmPulse2;                           // авария (2-я фаза мигания)
    static QPen * PenIzsRound;                              // перо замыкания / блокировки
    static QPen * PenExpired;                               // ТС устарели

    static QFont * font;                                    // шрифт отрисовки названия

    struct   StrlShapeProp * prop;                          // описание актуального набора геометрии
    class   Strl* sprStrl;                                  // стрелка
    class   Rc  * sprRc;                                    // РЦ под стрелкой
    QVector<class LinkedStrl*> strl;                        // определяющие стрелки
    bool	plusNormal;         							// TRUE - плюс по основному ходу, FALSE - ответвление
    QString name;                                           // имя (обозначение) стрелки
    int     idrc;											// N РЦ стрелки
    bool	b45;											// прорисовка под 45 град

    QPolygonF pathForPlus;                                  // путь для отрисовки тела стрелки в плюсе
    QPolygonF pathForMinus;                                 // путь для отрисовки тела стрелки в минусе
    QPolygonF pathNormalPlus;                               // путь для отрисовки тела стрелки в плюсе
    QPolygonF pathNormalMinus;                              // путь для отрисовки тела стрелки в минусе
    QTextOption * option;                                   // опции отрисовки номера (выравнивание)
    QPointF xyText;                                         // точка отрисовки текста
    QSize   tSize;                                          // размер поля для номера
    QRect     boundRect;                                    // прямоугольник для отрисовки окантовки

    void    DrawUndefined (QPainter*, bool gryclr, DStation * st, class DRailwaySwitch * strl);	// неопределенное положение стрелки
    bool    isStrlOk()                                      // проверка нахождения определяющих стрелок в требуемом положении
    {
        return LinkedStrl::checkRqSts(strl)==0;
    }

    // состояния визуализации
    bool isPlus     () { return (*state)[StsPlus    ]; }    // +
    bool isMinus    () { return (*state)[StsMinus   ]; }    // -
    bool isBusy     () { return (*state)[StsBusy    ]; }    // занятость
    bool isZmk      () { return (*state)[StsZmk     ]; }    // замыкание
    bool isRqRoute  () { return (*state)[StsRqRoute ]; }    // в устанавливаемом маршруте
    bool isPzdRoute () { return (*state)[StsPzdRoute]; }    // в поездном маршруте
    bool isMnvRoute () { return (*state)[StsMnvRoute]; }    // в маневровом маршруте
    bool isPassed   () { return (*state)[StsPassed  ]; }    // пройдена
    bool isIr       () { return (*state)[StsIr      ]; }    // ИР
    bool isOtu      () { return (*state)[StsOtu     ]; }    // OТУ
    bool isAlarm    () { return (*state)[StsAlarm   ]; }    // взрез, потеря контроля

    void drawRect(QPainter* painter, QPen pen);             // отрисовка квадратной окантовки стрелки

public:
    static void InitInstruments();                          // инициализация статических инструментов отрисовки

    ShapeStrl(QString& src, ShapeSet* parent);
    ~ShapeStrl();

    inline QString Name() { return name; }                  // имя
    inline short GetIdRc() {return idrc; }                  // РЦ
    class Route * ActualRoute() { return sprRc == nullptr ? nullptr : sprRc->ActualRoute(); }

    virtual void Draw(QPainter* pDC);						// функция рисования
    virtual void  Parse(QString&);
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void  Prepare();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    void accept();                                          // вычисление состояния примитива
};

#endif // SHAPESTRL_H





/*
//////////////////////////////////////////////////////////////////////////////////////
/// \brief The DShapeStrl class
///
class DShapeStrl: public DShape 							// Стрелка
{
protected:

struct StrlPropTagEx										// полное описание координат стрелки
{
    int	Align;		// выравнивание
    int x0;			// х надписи
    int y0;			// y надписи

    int x1,y1;		// х1,y1 начало тонкой линии плюсового положения по прямому ходу
    int x2,y2;		// х2,y2 конец  тонкой линии плюсового положения по прямому ходу
    int _x1,_y1;	// х1,y1 начало тонкой линии плюсового положения на ответвление
    int _x2,_y2;	// х2,y2 конец  тонкой линии плюсового положения на ответвление
    //
    int xx1,yy1,	// начало общего отрезка
        xx2,yy2,	// конец общего отрезка - начало разветвления
        xx3,yy3,	// конец отрезка плюсового положения по прямому ходу
        xx4,yy4;	// конец отрезка плюсового положения на ответвление

};

//static StrlPropTag StrlProp[nMaxStrlType];                // выравнивание и смещение надписи по типам стрелок
static StrlPropTagEx StrlProp60[nMaxStrlType];				// 60 градусов
static StrlPropTagEx StrlProp45[nMaxStrlType];				// 45 градусов

static	short	mLenX;										// длина по горизонтали
static	short	mLenY;										// длина по вертикали
static	short	dx;
static	short	dy;

static 	CFont	Font;										// Шрифт обозначения
static 	CFont	FontBold;									// Шрифт обозначения жирный
static	short	mThickH;									// толщина горизонтали
static	short	mThickA;									// толщина наклонной
static 	QColor TxtClr;									// Цвет надписи

public:
static	inline short GetThickH(){return mThickH;			 }
static	inline void  SetThickH(short w) { mThickH = w;		 }
static	inline short GetThickA(){return mThickA;			 }
static	inline void  SetThickA(short w) { mThickA = w;		 }
};
*/

