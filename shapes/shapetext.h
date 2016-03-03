#ifndef SHAPETEXT_H
#define SHAPETEXT_H

#include "shape.h"

class ShapeText : public DShape
{
protected:
    // типы шрифтов
    enum FamilyIds
    {
        Arial,              // 0
        Courier_New,        // 1
        System,             // 2 Consolas
        Times_New_Roman,    // 3
        MS_Sans_Serif,      // 4
        Fixedsys,           // 5 Consolas
        Small_Fonts,        // 6 Calibri
        Tahoma,             // 7
        Verdana,            // 8
    };

    static QStringList FontIds;
    static QPen   PenExpired;                               // устарели ТС
    static QPen   PenUndefined;                             // нет данных
public:
    //ShapeText();
    ShapeText (QString& src, class ShapeSet* parent);
    ~ShapeText();
    static void InitInstruments();                          // инициализация статических инструментов отрисовки

    virtual void  Draw (QPainter*);                         // функция рисования
    virtual void  Parse(QString&);                          // разбор строки описания

protected:
    int height;
    int width;
    FamilyIds familyId;
    bool bold;
    bool italian;

    QString text;                                           // собственно текст
    QString condition;                                      // условие, при котором текст отрисовывается
    class BoolExpression * exprCondition;                   // выражение для условия
    int     noRc;                                           // номер РЦ
    class Rc * sprRc;                                       //
    QFont   font;                                           // шрифт
    QColor  color;                                          // цвет
    QPen    pen;
    QTextOption option;                                     //

    bool IsNameSt() { return type == NAME_COD; }            // проверка, является ли текст именем станции

    void accept();                                          // вычисление состояния примитива
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

};

#endif // SHAPETEXT_H
