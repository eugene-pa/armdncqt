#ifndef DSHAPE_H
#define DSHAPE_H

#include <QGraphicsItem>
#include <QColor>
#include <QPainter>

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
	END_COD
};

class DShape : public QGraphicsItem
{
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

    class Status * state;                                   // класс статус
    void        setDimensions();
    void        log (QString msg);
public:
    DShape(QString& src, ShapeSet* parent);					// конструктор без параметров
    virtual ~DShape();										// конструктор без параметров

    static class ColorScheme * colorScheme;                 // цветовые схемы
    static void InitInstruments(QString, class Logger& );   // вызов инициализации статических инструментов отрисовки для асех примитивов
    static	qreal	mThick;									// толщина
//  static	bool	bUpdateOnlyMode;						// отрисовка только обновлений
//  static	bool	bRqRefreshPage;							// запрос на полную перерисовку страницы

    inline  QColor    SetColor (QColor clr) { colorPrev = color; color = clr; return colorPrev;}
    inline  QColor    GetColor () { return color; }
    inline  ShapeType GetType  () { return type; }
    inline short   GetNoSt() { return idst; }				// номер станции
    inline bool    IsVisible () { return visible; }         // видимый
    inline bool	   IsBlinking() { return blinking; }        // мигает
    inline bool	   IsSelected() { return selected; }        // выделен

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

/*
//////////////////////////////////////////////////////////////////////////////////////
class DShapeAngle: public DShape							// Съезд
{
protected:
static	short	mThick;										// толщина
public:
	DShapeAngle();											// конструктор
    void	Draw (QPainter *);								// функция рисования
    virtual void  Parse(QString)= 0;
    //void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
    virtual bool  CheckIt();
	virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
static	inline short GetThick(){return mThick;				 }	
static	inline void  SetThick(short w) { mThick = w;		 }

};




//////////////////////////////////////////////////////////////////////////////////////
//#define MAX_TRNSP 30										// Максимально возможное число транспарантов
// Интересный факт: при исп-ии QString возникают проблемы при удалении
// массива TrnspPropStruct[]
struct TrnspPropStruct
{
    QString		Name;										// имя транспаранта
    QString		NameTs;										// имя определяющего сигнала ТС
    bool		Programm;   								// признак программной логики обработки
//	bool		Pulse;										// признак использования мигающего сигнала
    QColor      ClrTrue;									// Цвет символов при наличии сигнала ТС
    QColor      ClrBkTrue;									// Цвет фона при наличии сигнала ТС
    bool		IsDrawFalse;								// флаг рисования при отсутствии сигнала
    QColor      ClrFalse;									// Цвет символов при отсутствии сигнала ТС
    QColor      ClrBkFalse;									// Цвет фона при отсутствии сигнала ТС
	short		cX;											// ширина
	short		cY;											// высота

    QString     NameIcon;                                   // имя иконки
    QBitmap*	pBmpRed;									// битовая карта пасс.сост.
    QBitmap*	pBmpYel;									// битовая карта акт. сост.
    QBitmap*	pBmpGrn;									// битовая карта акт. сост.
	void ReplaceSysColors();
    QBitmap * MakeBitmapCompatible (QPainter *, QBitmap *);
	
    QString     mDescription;                               // 2010.03.04-2010.06.04 Описание (тултип запоминаем)
};

const short TRNSP_TU      = 4 ;
const short TRNSP_ADODD	  = 5;
const short TRNSP_ADEVN   = 6;
const short TRNSP_DAY	  = 7;
const short TRNSP_DSN	  = 8;
const short TRNSP_ST_MODE = 11;
const short TRNSP_RON	  = 12;
const short TRNSP_ROCH	  = 13;
const short TRNSP_DIR_LFT = 14;								// направление налево (для слепых перегонов)
const short TRNSP_DIR_RHT = 15;								// направление направо(для слепых перегонов)
const short TRNSP_AUM_LFT = 16;								// АУМ налево 
const short TRNSP_AUM_RHT = 17;								// АУМ направо
const short TRNSP_TS      = 22;
const short TRNSP_UKSPS   = 23;
const short TRNSP_TEXT    = 25;
const short TRNSP_RSRV_KP = 28;
const short TRNSP_LINK    = 29;
const short TRNSP_DISKA   = 30;
const short TRNSP_DISKT   = 31;
const short TRNSP_EVNSVTFER=32;								// ЧКЗ
const short TRNSP_EVNKEY  = 33;								// ЧКЖ
const short TRNSP_ODDSVTFER=34;								// НКЗ
const short TRNSP_ODDKEY  = 35;								// НКЖ
const short TRNSP_BLDNG   = 36;
const short TRNSP_IR	  = 40;
const short TRNSP_VZ	  = 41;
const short TRNSP_IZS	  = 42;
const short TRNSP_NGBR	  = 43;
const short TRNSP_VSA     = 44;
const short TRNSP_CHDK	  = 45;
const short TRNSP_LAMP_WHT= 38;
const short TRNSP_LAMP_RED= 48;
const short TRNSP_LAMP_GRN= 49;
const short TRNSP_LAMP_YEL= 50;
const short TRNSP_PRG_LFT = 51;								// занятость перегона налево 
const short TRNSP_PRG_RHT = 52;								// занятость перегона направо
const short TRNSP_WORKING = 53;
const short TRNSP_BUTTON  = 64;

class DShapeTrnsp : public DShape							// ТРАНСПАРАНТ
{
public:
static void Release();										// освобождение ресурсов
static void	LoadSprBd();        							// функция загрузки справочника из БД
static void	adoLoadSprBd();         						// функция загрузки справочника из БД
static bool	bBmpTsFill;                 					// индикатор состояния ТС - заполненный
protected:

static TrnspPropStruct * Properties;						// указатель на массив описателей типов транспарантов
static short	AllTrnsp;									// статическая переменная - число транспарнтов
static QFont	Font;										// Шрифт написания

    short	subtype;										// тип транспаранта = номер-1
	short	mW;												// ширина
	short	mH;												// высота
	short	mX2;											// начальные координаты
	short	mY2;

    QString	mTsName1;                                       // имя сигнала 1
    QString	mTsName2;                                       // имя сигнала 2
    QString	mTsName3;                                       // имя сигнала 3
    QString	mTsNamePulse;                                   // 2007.09.21. Имя сигнала, определяющего мигание
    QString	mTuNameOn ;                                     // имя ТУ включить
    QString	mTuNameOff;                                     // имя ТУ отключить
    QString	mHelperOn ;                                     // имя меню включить
    QString	mHelperOff;                                     // имя меню отключить
    QString	mDescription;                                   // 2010.03.04. Описание

    bool	OneNameExpression;								// признак "одноименного" выражения (просто ТС)
    bool	Pulsing;										// пульсирует при истине в основном выражениии
	short	mNoRc;
	
    bool	m_bComment;										// признак привязки текста комментария
    bool	m_bOtu;											// признак транспаранта ОТУ

inline short   GetCod	    () { return subtype+1; }			// получить номер (см.БД) транспаранта
inline QString& GetNamePtr   () { return Properties[subtype].Name		; }
inline QString& GetNameTsPtr () { return Properties[subtype].NameTs	; }
inline bool     IsProgramm   () { return Properties[subtype].Programm	; }
inline QColor GetClrTrue  () { return Properties[subtype].ClrTrue	; }
inline QColor GetClrBkTrue() { return Properties[subtype].ClrBkTrue	; }
inline QColor GetClrFalse () { return Properties[subtype].ClrFalse	; }
inline QColor GetClrBkFalse(){ return Properties[subtype].ClrBkFalse; }

public:
	DShapeTrnsp();											// конструктор
    void	Draw (QPainter* );								// функция рисования
    virtual void  Parse(QString);
	void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника

    virtual bool  CheckIt();
	virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();


    QString& GetTsName   () { return mTsName1  ;}
    QString& GetTuNameOn () { return mTuNameOn ;}									// имя ТУ включить
    QString& GetTuNameOff() { return mTuNameOff;}									// имя ТУ отключить
    QString& GetHelperOn () { return mHelperOn ;}								// имя меню включить
    QString& GetHelperOff() { return mHelperOff;}								// имя меню отключить
virtual void  Prepare();

    int		GetTsStatus(QString * pstrExp = NULL);
    bool	GetColorOrBmp(QBitmap&, QColor&, QColor&);
inline bool		IsThisName	 (QString name) { return mTsName1==name; }

inline bool     IsTrnspStMode() { return GetCod()== TRNSP_ST_MODE	; }
inline bool     IsTrnspDirLft() { return GetCod()==TRNSP_DIR_LFT	; }
inline bool     IsTrnspDirRht() { return GetCod()==TRNSP_DIR_RHT	; }
inline bool     IsTrnspTs	 () { return GetCod()== TRNSP_TS		; }
inline bool     IsTrnspTu	 () { return GetCod()== TRNSP_TU		; }
inline bool     IsTrnspRsrv	 () { return GetCod()== TRNSP_RSRV_KP	; }
inline bool     IsTrnspLink	 () { return GetCod()== TRNSP_LINK		; }
inline bool     IsTrnspDiskA () { return GetCod()== TRNSP_DISKA		; }
inline bool     IsTrnspDiskT () { return GetCod()== TRNSP_DISKT		; }
inline bool     IsTrnspDiskTOdd () { return GetCod()== TRNSP_DISKT && mTsName1.indexOf("ТН") >= 0; }
inline bool     IsTrnspDiskTEvn () { return GetCod()== TRNSP_DISKT && mTsName1.indexOf("ТЧ") >= 0; }
inline bool		IsTrnspEvnKey() { return GetCod()== TRNSP_EVNKEY	; }
inline bool		IsTrnspOddKey() { return GetCod()== TRNSP_ODDKEY	; }
inline bool     IsTrnspIr	 ()	{ return GetCod()== TRNSP_IR		; }
inline bool     IsTrnspVz	 ()	{ return GetCod()== TRNSP_VZ		; }
inline bool     IsTrnspIzs	 ()	{ return GetCod()== TRNSP_IZS		; } // ЗС,ЗСН,ЗСЧ
inline bool     IsTrnspNgbr	 () { return GetCod()== TRNSP_NGBR		; }
inline bool     IsTrnspVsa	 ()	{ return GetCod()== TRNSP_VSA		; }
inline bool     IsTrnspBldng () { return GetCod()== TRNSP_BLDNG		; }
inline bool     IsTrnspChdk  () { return GetCod()== TRNSP_CHDK		; }
inline bool     IsTrnspChdkOdd() { return GetCod()== TRNSP_CHDK	&& mTsName1.indexOf("Н") >= 0; }
inline bool     IsTrnspChdkEvn() { return GetCod()== TRNSP_CHDK	&& mTsName1.indexOf("Ч") >= 0; }
inline bool     IsTrnspAD	  () { return GetCod()== TRNSP_ADODD || GetCod()== TRNSP_ADEVN; }
inline bool     IsTrnspDay    () { return GetCod()== TRNSP_DAY		; }
inline bool     IsTrnspDsn    () { return GetCod()== TRNSP_DSN		; }
inline bool		IsTrnspLamp	  () { return ((GetCod()>=TRNSP_LAMP_RED && GetCod()<=TRNSP_LAMP_YEL) || GetCod()==TRNSP_LAMP_WHT); }
inline bool		IsTrnspLampRir() { return IsTrnspLamp() && IsThisName("УМ-РИР"); }
//inline bool		IsTrnspText	  () { return GetCod() == TRNSP_TEXT || strlen(GetNamePtr())==0; }
inline bool		IsTrnspText	  () { return GetCod() == TRNSP_TEXT || GetNamePtr().length()==0; }
inline bool		IsTrnspLampUrz   () { return IsTrnspLamp() && IsThisName("УРЗ" ); }
inline bool		IsTrnspLampUrzOdd() { return IsTrnspLamp() && IsThisName("УРЗН"); }
inline bool		IsTrnspLampUrzEvn() { return IsTrnspLamp() && IsThisName("УРЗЧ"); }
inline bool		IsTrnspWorking   () { return GetCod()== TRNSP_WORKING; }
inline bool		IsTrnspArrow     () { return GetCod()== TRNSP_AUM_LFT || GetCod()== TRNSP_AUM_RHT ||
											 GetCod()== TRNSP_PRG_LFT || GetCod()== TRNSP_PRG_RHT; }
inline bool		IsTrnspButton	 () { return GetCod()==TRNSP_BUTTON; }
inline bool		IsComment		 () { return m_bComment; }
inline void		SetWidth(short w)   { mW = w; }
};
*/

//////////////////////////////////////////////////////////////////////////////////////
class DShapeNameSt : public DShape							// НАИМЕНОВАНИЕ СТАНЦИИ
{
protected:
static QFont	Font;										// Шрифт написания
//	short	mNoSt;											// номер станции
	char	mNameSt[64];									// имя станции
	int		mWtxt;
	int		mHtxt;
public:
	DShapeNameSt();											// конструктор
    void	Draw (QPainter*);								// функция рисования
    virtual void  Parse(QString&);
	void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
    virtual bool  CheckIt();
	virtual void  FixUpUnsafe();
    virtual QString Dump ();
};


//////////////////////////////////////////////////////////////////////////////////////
#define MAXFONTS 9
class DShapeText : public DShapeNameSt						// ТЕКСТ
{
protected:

static QVector<QString> AvailFonts;                         // имена шрифтов

	short	mH;												// Высота шрифта
	short	mW;												// Ширина шрифта
	short	mFont;
    bool	mBold;
    bool	mItal;
	int		mWtxt;
	int		mHtxt;
	char	mExpr[128];										// Условие на отображение
	short	mNoRc;
	class	DStation * pSt;									// указатель на справочник станции
public:

    static void ReadFonts(QString * bdpath);                // БД

	DShapeText();
    void	Draw (QPainter *);								// функция написания
    virtual void  Parse(QString&);
	void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
	virtual void  Prepare();
    virtual bool  CheckIt();
	virtual void  FixUpUnsafe();
    virtual QString Dump ();
};


//////////////////////////////////////////////////////////////////////////////////////
class DShapeBox : public DShape								// ПРЯМОУГОЛЬНИК
{
protected:
//	short	mNoSt;											// номер станции	
	short	mX2;											// начальные координаты
	short	mY2;
public:	
	DShapeBox();
    void	Draw(QPainter *);								// функция написания
    virtual void  Parse(QString&);
	void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
    virtual QString Dump ();
};


//////////////////////////////////////////////////////////////////////////////////////
class DShapePrzd : public DShape			// ПЕРЕЕЗД
{
protected:
	short	mX2;							// конечные координаты
	short	mY2;
//	short	mNoSt;							// номер станции
public:
static	short	mW;							// ширина изображения переезда
	class DPereezd * SprPrzd;				// 2015.01.21. указатель на справочник переезда из БД
	char	mName1[64];						// выражение для контроля открытия переезда
	char	mName2[64];						// выражение для контроля аварии 1
	char	mName3[64];						// выражение для заградительной сигнализации 
	char	mName4[64];						// выражение для контроля извещения
	char	mName5[64];						// выражение для контроля извещения 2
    bool	bOneName1;						// признак "односложного (один ТС) выражения для mName1
    bool	bOneName2;						// признак "односложного (один ТС) выражения для mName2
    bool	bOneName3;						// признак "односложного (один ТС) выражения для mName3
    bool	bOneName4;						// признак "односложного (один ТС) выражения для mName4
    bool	bOneName5;						// признак "односложного (один ТС) выражения для mName5
	DShapePrzd();							// Конструктор
    void	Draw(QPainter *);				// функция написания
    virtual void  Parse(QString&);
	void	GetArea(short*,short*,short*,short*);			// вычисление замещаемого прямоугольника
virtual void  Prepare();
static	inline short GetW()			{return mW;}	
static	inline void  SetW(short w)	{ mW = w;  }
virtual QString Dump ();
virtual QString ObjectInfo();

};


// класс для хранения и доступа к свойствам объектов
// свойства описываются перечислением STATUS_TYPES, индексы 0,1 зарезервированы под STS_EXPIRE, STS_UNDEFINED
// доступ осуществляется по индексу свойства: [indx]
// изменение свойсва - функция setSts (int i, bool s)
class  Status
{
protected:
    //unsigned int sts;                                       // состояния пользователя
    QBitArray   sts;
    #define max_indx 32

public:
    enum STATUS_TYPES
    {
        StsExpire    = 0,
        StsUndefined = 1,
    };

    Status()
    {
        //sts  = 0;
        sts.fill(false, 32);
    }

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
