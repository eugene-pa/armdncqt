#ifndef SHAPESTRL_H
#define SHAPESTRL_H

#include "shape.h"

class ShapeStrl : public DShape
{
public:
    enum StrlTypes          // типы стрелок
    {
                            // горизонтали вбок
        WEtoNE      = 0,    // горизонталь запад > восток ответвление на северо восток
        EWtoNW      = 1,    // горизонталь восток > запад ответвление на северо запад
        WEtoSE      = 2,    // горизонталь запад > восток ответвление на юго    восток
        EWtoSW      = 3,    // горизонталь восток > запад ответвление на юго    запад

                            // наклонные вправо / влево
        SWNEtoE     = 4,    // наклонная юго-запад  >  северо восток ответвление на восток
        SENWtoW     = 5,    // наклонная юго-восток >  северо западк ответвление на запад
        NWSEtoE     = 6,    // наклонная северо-запад  >  юго-восток ответвление на восток
        NESWtoW     = 7,    // наклонная северо-восток >  юго-запад  ответвление на западк

                            // вертикали вбок
        SNtoNE      = 8 ,   // вертикаль юг  >  север ответвление на северо восток
        SNtoNW      = 9 ,   // вертикаль юг  >  север ответвление на северо запад
        NStoSE      = 10,   // вертикаль север > юг ответвление на юго восток
        NStoSW      = 11,   // вертикаль север > юг ответвление на юго запад

                            // разветвления
        WtoNEorSE   = 12,   // разветвление вправо с запада  на северо восток и юго восток
        EtoNWorSW   = 13,   // разветвление влево  с востока на северо запад и юго запад

                            // наклонные вверх / вниз
        SWNEtoN     = 14,   // наклонная юго-запад > северо-восток ответвление на север
        SENWtoN     = 15,   // наклонная юго-восток > северо-запад ответвление на север
        NWSEtoS     = 16,   // наклонная северо-запад > юго-восток ответвление на юг
        NESWtoS     = 17,   // наклонная северо-восток > юго-запад ответвление на юг

        nMaxStrlType = 18,  // число типов стрелок
    };

protected:
    Status State;                                           // статус отображения отрезка РЦ
                                                            // важно: может не полностью совпадать со статусом объекта РЦ,
                                                            // т.к., например, есть отрезки, не проходящие по стрелкам
    class   Strl* sprStrl;                                  // стрелка
    class   Rc  * sprRc;                                    // РЦ под стрелкой
    QVector<class LinkedStrl*> strl;                        // определяющие стрелки
    bool	plusNormal;         							// TRUE - плюс по основному ходу, FALSE - ответвление
    QString name;                                           // имя (обозначение) стрелки
    int     idrc;											// N РЦ стрелки
    bool	b45;											// прорисовка под 45 град

    void    DrawUndefined (QPainter*, bool gryclr, DStation * st, class DRailwaySwitch * strl);	// неопределенное положение стрелки
    bool    isStrlOk();                                     // проверка нахождения определяющих стрелок в требуемом положении
public:
    ShapeStrl(QString& src, ShapeSet* parent);
    ~ShapeStrl();

    inline QString Name() { return name; }                  // имя
    inline short GetIdRc() {return idrc; }                  // РЦ

    virtual void Draw(QPainter* pDC);						// функция рисования
    virtual void  Parse(QString&);
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void  Prepare();
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

