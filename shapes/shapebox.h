#ifndef SHAPEBOX_H
#define SHAPEBOX_H


class ShapeBox : public DShape
{
public:
    ShapeBox(QString& src, ShapeSet* parent);
    ~ShapeBox();

    static QPen pen;                                        // перо
    static void InitInstruments();                          // инициализация статических инструментов отрисовки
    virtual void  Draw (QPainter*)  override;               // функция рисования

protected:
    void accept() override;                                 // вычисление состояния примитива
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
    virtual void Parse(QString&) override;                  // разбор строки описания

};

#endif // SHAPEBOX_H
