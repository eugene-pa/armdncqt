#ifndef SHAPESVTF_H
#define SHAPESVTF_H

#include "shape.h"


class ShapeSvtf : public DShape
{
protected:
    // статические ресурсы для отрисовки
    static QPen * MainPen;                                  // перо основное
    static QPen * MainPen2;                                 // перо двойной линии
    static QPen * PenUndefined;                             // неопред.состояние
    static QPen * PenExpired;                               // устарели ТС
    static QPen * PenAlarm;                                 // авария светофора

    static QBrush * BrushPzdOn;                             // поездной открыт
    static QBrush * BrushYelllow;                           // желтый
    static QBrush * BrushPzdOff;                            // поездной закрыт
    static QBrush * BrushPzdInOff;                          // поездной входной закрыт
    static QBrush * BrushMnvOn;                             // маневровый открыт
    static QBrush * BrushMnvOff;                            // маневровый закрыт
    static QBrush * BrushUndefined;                         // неопред.состояние
    static QBrush * BrushExpired;                           // устарели ТС
    static QBrush * BrushLockBackground;                    // фон при блокировке
    static QBrush * BrushAdBackground;                      // фон при автодействии
    static QBrush * BrushOmBackground;                      // фон при отмене маршрута
//  static QBrush * BrushLocked;

    static QFont * font;                                    // шрифт отрисовки названия

    // описание геометрии светофора
    struct SvtfPropTag
    {
        int bx1,by1,bx2,by2;                                // основание (base)
        int hx1,hy1,hx2,hy2;                                // ножка
        int cx,cy,d;                                        // сентр и диаметр
    };

    enum SvtfTypes
    {
        PzdnLR      = 0,		                            //	0   |--  поездной       Left To Right
        PzdnRL      = 1,		                            //	1   --|  поездной       Right To Left
        MnvrLR      = 2,		                            //	2   |--  маневровый     Left To Right
        MnvrRL      = 3,		                            //	3   --|  маневровый     Right To Left
        PzdnMnvrLR  = 4,		                            //	4   |--  совмещенный    Left To Right
        PzdnMnvrRL  = 5,		                            //	5   --|  совмещенный    Right To Left
        RepeteLR    = 6,	    	                        //	6   |--  повторитель    Left To Right
        RepeteRL   = 7,		                                //	7   --|  повторитель    Right To Left

        Pzdn        = 8,		                            // 8    поездной без ножки
        Mnvr        = 9,		                            // 9    маневровый без ножки
        Base_E      = 10,		                            // 10   E
        Base_NE     = 11,		                            // 11	NE
        Base_N      = 12,		                            // 12	N
        Base_NW     = 13,		                            // 13	NW
        Base_W      = 14,		                            // 14	W
        Base_SW     = 15,		                            // 15	SW
        Base_S      = 16,		                            // 16	S
        Base_SE     = 17,		                            // 17	SE
        MaxSvtfType = 18,                                   // число типов светофоров
    };

    enum
    {
        StsAlarm = 2,                                       // авария
        StsBlock,                                           // блокирован
        StsPzdRoute,                                        // в поездном маршруте
        StsMnvRoute,                                        // в маневровом маршруте
        StsOpenPzd,                                         // открыт поездной
        StsOpenMnv,                                         // открыт маневровый
        StsYellow,                                          // желтый разрешающий
        StsPrgls,                                           // пригласительный
        StsAD,                                              // автодействие
        StsOM,                                              // отмена маршрута
    };

    // состояния визуализации
    bool isAlarm    () { return (*state)[StsAlarm   ]; }    // авария
    bool isBlock    () { return (*state)[StsBlock   ]; }    // блокирован
    bool isPzdRoute () { return (*state)[StsPzdRoute]; }    // в поездном маршруте
    bool isMnvRoute () { return (*state)[StsMnvRoute]; }    // в маневровом маршруте
    bool isOpenPzd  () { return (*state)[StsOpenPzd ]; }    // открыт поездной
    bool isOpenMnv  () { return (*state)[StsOpenMnv ]; }    // открыт маневровый
    bool isYellow   () { return (*state)[StsYellow  ]; }    // желтый разрешающий
    bool isPrgls    () { return (*state)[StsPrgls   ]; }    // пригласительный
    bool isAD       () { return (*state)[StsAD      ]; }    // автодействие
    bool isOM       () { return (*state)[StsOM      ]; }    // отмена маршрута

    static SvtfPropTag svtfProp[MaxSvtfType];
    static int diametr;                                     // диаметр
    static int baseW  ;                                     // ширина основания
    static int standH ;                                     // высота ножки

    SvtfPropTag * prop;
    QString name;                                           // имя (обозначение) светофора

    QTextOption * option;                                   // опции отрисовки номера (выравнивание)
    QPointF xyText;                                         // точка отрисовки текста
    QSize   tSize;                                          // размер поля для имени
    QPointF center;                                         // центр светофора
    qreal   r;                                              // радиус (строго говоря, может задаваться настройками!)
    QRectF  backText;                                       // окантовка текста (для выделения)

//    int idSvtf1,                                            // ID первого светофора из описания
//        idSvtf2;                                            // ID второого светофора из описания
    class Svtf * svtf;                                      // светофор поездной   или nullptr
    class Svtf * svtfM;                                     // светофор маневровый или nullptr

public:
    ShapeSvtf(QString& src, ShapeSet* parent);              // конструктор
    ~ShapeSvtf();

    static void InitInstruments();

    inline QString Name() { return name; }                  // имя

    virtual void  Draw (QPainter*);                         // функция рисования
    virtual void  Parse(QString&);                          // разбор строки описания
    virtual bool  CheckIt();
    virtual void    FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void    Prepare();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    virtual void accept();                                  // вычисление состояния примитива

static	inline short GetThick() { return mThick;	}
static	void  SetThick(short w, bool bUpdateDefault = true);

};

#endif // SHAPERC_H

/*
class DShapeSvtf: public DShape 							// Светофор
{
protected:
struct SvtfPropTag
{
    int ox1,oy1,ox2,oy2;				// овал
    int bx1,by1,bx2,by2;				// основание (base)
    int hx1,hy1,hx2,hy2;				// ножка
};
    static 	SvtfPropTag SvtfProp[nMaxSvtfType];


static short	mDiam;										// Диаметр светофора
static 	short	mH;											// высота ножки светофора
static 	short	mW;											// ширина основания ножки светофора
static 	CFont	Font;										// Шрифт обозначения
static 	CPen	Pen;										// Перья
static 	CPen	PenYel;										//
static 	CPen	PenRed;										//
static 	CPen	PenGrn;										//
static 	CPen	PenWht;										//

static 	CPen	PenLocked;									// Перо отключенного светофора
static 	CBrush	PzdOnClr;									// Цвет поездного открытого
static 	CBrush	PzdOnClrYel;								// Цвет поездного открытого желтого  (желтый)	2009.01.20
static 	CBrush	PzdOffClr;									// Цвет поездного закрытого
static 	CBrush	PzdInOffClr;								// Цвет входного поездного закрытого (красный)	2009.01.20
static 	CBrush	MnvOnClr;									// Цвет поездного открытого
static 	CBrush	MnvOffClr;									// Цвет поездного закрытого
static 	CBrush	LockedClr;									// Отключен
static	short	mThick;										// толщина	CShape(short x, short y);
static 	QColor TxtClr;									// Цвет надписи
static 	QColor LnClr;										// Цвет линий
    //char	mName[10];										// наименование светофора
    CString mName;                                          // наименование светофора

    int		StatusExtPrev;									// сост.на пред.шаге
public:
    DShapeSvtf();											// конструктор
    void	Draw(CDC* pDC);									// функция рисования
    inline short GetSize(){return sizeof(DShapeSvtf);};		// функция получения длины примитива
    //virtual void  SetInfo(char *p)= 0;					// ввод инфо из строки
    virtual void  Parse(QString)= 0;
    void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual CString Dump();
    virtual CString ObjectInfo();

        //inline LPCTSTR GetName() { return mName; }
        inline CString& GetName() { return mName; }
        inline virtual short GetNo()	{ return id ? id : mNoExt; }
        inline virtual short GetExtNo() { return mNoExt; }
        inline BOOL IsManevr  () { return subtype==MNVR_EVN || subtype==MNVR_ODD; }
        inline BOOL IsTrain   () { return subtype==PZDN_EVN || subtype==PZDN_ODD;  }
        inline BOOL IsCombine () { return subtype== TWO_EVN || subtype== TWO_ODD; }

    static	inline void  CreatePen()
    {
#ifdef QT

#else   // #ifdef QT
        Pen.CreatePen(PS_SOLID,mThick,LnClr);
#endif  // #ifdef QT
    }

    static	inline short GetThick(){return mThick;				 }
    static	inline void  SetThick(short w)
    {
        mThick=w; mH=w+2;
#ifdef QT

#else   // #ifdef QT
        Pen.DeleteObject(); CreatePen();
#endif  // #ifdef QT
    }
    static	inline short GetDiam (){return mDiam;				 }
    static	void  SetDiam (short w, bool bUpdateDefault = true);

};



*/
