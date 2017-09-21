#ifndef FRAMEMVV_H
#define FRAMEMVV_H

#include <QFrame>

namespace Ui {
class FrameMVV;
}

class FrameMVV : public QFrame
{
    Q_OBJECT

public:
    explicit FrameMVV(QWidget *parent = 0);
    ~FrameMVV();

    void SetNo(int n);                          // задать номер отображаемого МВВ

private:
    Ui::FrameMVV *ui;
    int no;                                     // номер МВВ (1 или 2, определяет группут отображаемых модулей)
    class QLed * modules[24];                   // массив укзателей на классы для каждого модуля из 24
};

#endif // FRAMEMVV_H
