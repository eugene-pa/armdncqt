#ifndef SHAPERC_H
#define SHAPERC_H

#include "shape.h"

class LinkedRc;

class ShapeRc : public DShape
{
    friend class ShapeTrain;
protected:
    class Rc * sprRc;                                       // указатель на справочник РЦ
    QVector<class LinkedStrl*> strl;                        // определяющие стрелки
    //QPainterPath path;                                    // путь для отрисовки смежных отрезков, реально хранится в одном из смежных отрезков
    QPolygonF path;                                         // путь для отрисовки смежных отрезков, реально хранится в одном из смежных отрезков

    bool combined;                                          // признак того, что РЦ объединена в составе пути этого или сопряженного отрезка


    static QPen *PenFree;                                   // свободная РЦ
    static QPen *PenBusy;                                   // занятая РЦ (если занятая РЦ замкнута - контур замыкания вокруг)
    static QPen *PenRqRoute;                                // в устанавливаемом маршруте
    static QPen *PenPzdRoute;                               // в поездном маршруте
    static QPen *PenMnvRoute;                               // в маневровом маршруте
    static QPen *PenZmk;                                    // замкнутая РЦ не в неиспользованном маршруте
    static QPen *PenZmkContur;                              // замкнутая РЦ для контура (рисуется поверх незаполненным контуром)
    static QPen *PenZmkConturMnv;                           // замкнутая РЦ для контура в маневровом маршруте(рисуется поверх незаполненным контуром)
    static QPen *PenIr;                                     // искусственная разделка (мигает поверх других состояний)
    static QPen *PenExpired;                                // ТС устарели
    static QPen *PenUndefined;                              // объект неопределен - пассивная отрисовка

    // перечисление типов(формы) примитива
    enum LineTypes
    {
        // Строго говоря, тип примитива важен только для его отрисовки и генерации
        Vert    = 0,                                        // вертикаль
        Horz    = 1,                                        // горизонталь
        ArcNW   = 2,                                        // дуга северо-запад
        ArcNE   = 3,                                        // дуга северо-восток
        ArcSE   = 4,                                        // дуга юго-восток
        ArcSW   = 5,                                        // дуга юго-запад
        ArcSWN  = 6,                                        // дуга на 3/4 юг-запад-север
        ArcNES  = 7,                                        // дуга на 3/4 север-восток-юг

        SWTONE  = 8,                                        // наклон слева снизу вправо вверх
        NWTOSE  = 9,                                        // наклон слева сверху вправо вниз
        Free    = 10,                                       // свободные координаты
    };

    enum                                                    // состояние примитива для визуализации (определяем по состоянию объекта с учетом стрелок)
    {
        StsBusy    = 2,                                     // занятость
        StsZmk        ,                                     // замыкание
        StsRqRoute    ,                                     // в устанавливаемом маршруте
        StsPzdRoute   ,                                     // в поездном маршруте
        StsMnvRoute   ,                                     // в маневровом маршруте
        StsPassed     ,                                     // пройдена
        StsIr         ,                                     // ИР
    };

public:
    ShapeRc(QString& src, ShapeSet* parent);                // конструктор
    ~ShapeRc();
    static void InitInstruments();                          // инициализация статических инструментов отрисовки

    virtual void  Draw (QPainter*);                         // функция рисования
    virtual void  Parse(QString&);                          // разбор строки описания
    virtual bool  CheckIt();
    virtual void  FixUpUnsafe();
    virtual QString Dump();
    virtual QString ObjectInfo();
    virtual void  Prepare();

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

    void normalize();

    // состояния визуализации
    bool isBusy     () { return (*state)[StsBusy    ]; }    // занятость
    bool isZmk      () { return (*state)[StsZmk     ]; }    // замыкание
    bool isRqRoute  () { return (*state)[StsRqRoute ]; }    // в устанавливаемом маршруте
    bool isPzdRoute () { return (*state)[StsPzdRoute]; }    // в поездном маршруте
    bool isMnvRoute () { return (*state)[StsMnvRoute]; }    // в маневровом маршруте
    bool isPassed   () { return (*state)[StsPassed  ]; }    // пройдена
    bool isIr       () { return (*state)[StsIr      ]; }    // ИР

    void accept();                                          // вычисление состояния примитива
    bool isStrlOk();                                        // проверка нахождения определяющих стрелок в требуемом положении    
    bool isStrlInRoute(Route* route);                       // проверка, удовлетворяет ли положение направляющих стрелок отрезка ЗАДАННОМУ положению стрелок указанного маршрута
                                                            // функция используется для прокладки трассы устанавливаемого маршрута, при этом фактическое положение стрелок может отличаться от заданного
//static	inline short GetThick(){return mThick;									}
//static	void  SetThick(short w, bool bUpdateDefault = true);

};

#endif // SHAPERC_H
