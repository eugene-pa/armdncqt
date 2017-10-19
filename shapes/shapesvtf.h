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
    static QPen * PenText;                                  // надпись

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
    static QBrush * BrushAlarmBackground;                   // фон при аварии
//  static QBrush * BrushLocked;

    static QFont * font;                                    // шрифт отрисовки названия

    static int fontsize;                                    // размер шрифта
    static int offset_x,                                    // смещение текста
               offset_y;
    static QString fontname;                                // шрифт

    // описание геометрии светофора
    struct SvtfPropTag
    {
        int bx1,by1,bx2,by2;                                // основание (base)
        int hx1,hy1,hx2,hy2;                                // ножка
        int cx,cy,d;                                        // центр и диаметр
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


    bool isMnv() { return subtype==MnvrLR || subtype==MnvrRL; }

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
    QRectF  boundRect;                                      // поле вывода текста
    QRectF  backRect;                                       // подложка текста
    QLineF  base;                                           // основание
    QLineF  stand;                                          // ножка

//    int idSvtf1,                                            // ID первого светофора из описания
//        idSvtf2;                                            // ID второого светофора из описания
    class Svtf * svtf;                                      // светофор поездной   или nullptr
    class Svtf * svtfM;                                     // светофор маневровый или nullptr

public:
    ShapeSvtf(QString& src, ShapeSet* parent);              // конструктор
    ~ShapeSvtf();

    static void InitInstruments();

    inline QString Name() { return name; }                  // имя

    virtual void  Draw (QPainter*) override;                // функция рисования
    virtual void  Parse(QString&) override;                 // разбор строки описания
    virtual bool  CheckIt() override;
    virtual void    FixUpUnsafe() override;
    virtual QString Dump() override;
    virtual QString ObjectInfo() override;
    virtual void    Prepare() override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
    virtual void accept() override;                          // вычисление состояния примитива

static	inline short GetThick() { return mThick;	}
static	void  SetThick(short w, bool bUpdateDefault = true);

};

#endif // SHAPERC_H

