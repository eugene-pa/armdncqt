#ifndef SHAPEPRZD_H
#define SHAPEPRZD_H

#include "shape.h"
#include "../spr/pereezd.h"

class ShapePrzd : public DShape
{
    friend class Pereezd;
protected:
    enum
    {
        exprOpen = 0,
        exprAlarm,
        exprBlock,
        exprIzv1,
        exprIzv2,
        exprAll,
    };

    enum                                                    // состояние примитива для визуализации (определяем по состоянию объекта с учетом стрелок)
    {
        StsOpen     = 2,                                    // открыт
        StsClose       ,                                    // закрыт
        StsNotifying   ,                                    // извещение 1 или 2
        StsAlarm       ,                                    // авария
        StsBlock       ,                                    // контроль заград.сигнализации
    };

    class BoolExpression * formulas[exprAll];               // формулы

    class Pereezd * pereerd;                                // справочник переезда
    static QBrush BrushOpen;                                // открыт
    static QBrush BrushClose;                               // закрыт
    static QBrush BrushUndef;                               // неизвестно
    static QBrush BrushExpired;                             // устарели ТС
    static QPen   PenOpen;                                  // открыт окантовка
    static QPen   PenClose;                                 // закрыт окантовка
    static QPen   PenUndef;                                 // неизвестно окантовка
    static QPen   PenExpired;                               // устарели ТС
    static QBrush BackgroundAlarm;                          // фон транспаранта аларма
    static QBrush ForeAlarm;                                // цвет транспаранта аларма
    static QFont  font;                                     // шрифт отрисовки А, ЗГ
    static int fontsize;                                    // размер шрифта
    static QString fontname;                                // шрифт
    static QTextOption option;                              // опции отрисовки номера (выравнивание)

    QRectF rectText;                                        // место отрисовки текста
    QRectF backRect;                                        // подложка текста

public:
    ShapePrzd(QString& src, ShapeSet* parent);
    ~ShapePrzd();
    static void InitInstruments();                          // инициализация статических инструментов отрисовки
    virtual void  Draw (QPainter*) override;                // функция рисования

protected:
    void accept() override;                                 // вычисление состояния примитива
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
    virtual void  Parse(QString&) override;                 // разбор строки описания
    virtual QString Dump() override;
};

#endif // SHAPEPRZD_H
