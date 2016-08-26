#ifndef DSHAPE_H
#define DSHAPE_H

#include <QGraphicsItem>
#include <QColor>
#include <QPainter>
//#include <QPolygonF>

#include "colorscheme.h"
#include "../spr/station.h"


// Базовые классы DShape, реализация реального времени

enum ShapeType
{
	STR_COD		=1 ,HOR_COD,VER_COD,BOX_COD,CHR_COD,APND_COD,
	INPUT_COD	=8 ,DYN_COD,MENU_COD,
	PRZD_COD	=15,
	SEGMENT_COD	=16,ANGLE_COD,SVTF_COD,STRL_COD,MODE_COD,
	NAME_COD	=22,
	TEXT_COD,
    TRAIN_COD,
	END_COD
};

enum TA
{
    _TA_LEFT  = 0,
    _TA_RIGHT = 1
};

class DShape : public QGraphicsItem
{
    friend class ShapeSet;

protected:
    class ShapeSet * set;                                   // класс родителя-собственника формы
    QString     source;                                     // исходная строка примитива
    ShapeType	type;   									// код примитива
    QColor   	color;										// активный цвет;
    QColor      colorBack;									// цвет окантовки (если 0 - нет окантовки)
    QColor      colorPrev;									// цвет отрисовки в предыдущем такте

    float		x1,y1;										// координаты начальной точки
    float       x2,y2;     									// координаты конечной точки
    float       width;                                      // ширина
    float       height;                                     // высота
    QRectF      rect;                                       //
    QPointF     XY;                                         // базовая точка

    int         idObj;                                      // N РЦ/стрелки/светофора/транспаранта
    int 		idst;										// номер станции
    class       Station * st;								// указатель на справочник станции, или 0
    int         subtype;									// тип отрезка/откоса/стрелки/светофора
    bool		blinking;									// примитив мигает
    bool		visible;									// отрисовывается по условиям
    bool		selected;									// отрисовывается по условиям

//  int			StatusPrev;									// сост.на пред.шаге
//  bool        mStrlOk;									// ПРИЗНАК ВХОЖДЕНИЯ ПО СТРЕЛКАМ
//  QColor      mPrvCol;									// цвет прорисовки в прошлый раз
//  short       mNstrl[3];									// 1,2,3 направляющие стрелки

    class Status * state;                                   // статус
    void        setDimensions();
    void        log (QString msg);
public:
    DShape(QString& src, ShapeSet* parent);					// конструктор без параметров
    virtual ~DShape();										// конструктор без параметров

    static class ColorScheme * colorScheme;                 // цветовые схемы
    static void InitInstruments(QString, class Logger& );   // вызов инициализации статических инструментов отрисовки для асех примитивов
    static	qreal	mThick;									// толщина
    static	bool    globalPulse;                            // eуправление миганием примитивов
//  static	bool	bUpdateOnlyMode;						// отрисовка только обновлений
//  static	bool	bRqRefreshPage;							// запрос на полную перерисовку страницы

    inline  QColor    SetColor (QColor clr) { colorPrev = color; color = clr; return colorPrev;}
    inline  QColor    GetColor () { return color; }
    inline  ShapeType GetType  () { return type; }
    inline short   GetNoSt() { return idst; }				// номер станции
    inline class Station * St() { return st; }              // станция
    inline bool    IsVisible () { return visible; }         // видимый
    inline bool	   IsBlinking() { return blinking; }        // мигает
    inline bool	   IsSelected() { return selected; }        // выделен

    virtual void accept() { }                               // вычисление состояния примитива
    virtual void  Draw(QPainter* painter);                  // отрисовка
    virtual bool  CheckIt() { return true; }				// Проверка корректности привязки
    virtual void  FixUpUnsafe   () { }						// Очистка небезопасных ошибок привязки
    virtual QString Dump		()  { return "?";  }
    virtual QString ObjectInfo	()  { return "";  }			// развернутая информация по объекту
    virtual void    Prepare     ()  { }
    virtual void    Parse(QString&)  { }
    virtual QRectF boundingRect() const { return rect; }
    virtual void paint (QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    virtual void  MoveRel(short dx,short dy);				// перемещение на заданное расстояние

    QPointF center() { return rect.center     (); }
    QPointF xy()     { return rect.topLeft    (); }
    QPointF  x2y2()  { return rect.bottomRight(); }
    QSizeF  size()   { return rect.size       (); }
//inline bool    SetVisivle(bool s=true) { bVisible = s; }	// 2009.08.24. Отрисовывается по условиям
};



// класс для хранения и доступа к свойствам объектов
// свойства описываются перечислением STATUS_TYPES, индексы 0,1 зарезервированы под STS_EXPIRE, STS_UNDEFINED
// доступ осуществляется по индексу свойства: [indx]
// изменение свойсва - функция setSts (int i, bool s)
class  Status
{
protected:
    #define max_indx 32

    QBitArray   sts;                                        // состояние
    QBitArray   stsPrv;                                     // состояние на предыдущем шаге

public:
    enum STATUS_TYPES
    {
        StsExpire    = 0,
        StsUndefined = 1,
    };

    Status()
    {
        sts   .fill(false, 32);                             // инициируем 0
        stsPrv.fill(true, 32);                              // инициируем 1 для гарантии несравнения
    }

    bool hasChanges() { return sts == stsPrv; }             // были ли изменения с прошлого раза
    bool IsAvailable (int i) { return i >= 0 && i < max_indx ; }    // проверка валидности индекса
    bool isExpire()             { return (*this)[StsExpire   ]; }
    bool isUndefined  ()        { return (*this)[StsUndefined]; }
    void setExpire    (bool s)  { set (StsExpire   , s); }
    void setUndefined (bool s)  { set (StsUndefined, s); }

    void set (int i, bool s)
    {
        if (IsAvailable(i))
            sts[i] = s;
    }

    // перегруженный индексатор индексации используется для проверки состояний
    bool operator[] (int i)
    {
        return IsAvailable(i) ? sts[i] : false;
    }



    // перегруженный оператор сравнения
    friend bool operator == ( const Status& left, const Status& right)
    {
        return left.sts == right.sts;
    }

    // перегруженный оператор присваивания
    Status& operator = (const Status& s)
    {
        if (this != &s)
        {
            sts          = s.sts;
        }
        return *this;
    }

};
#endif // DSHAPE_H
