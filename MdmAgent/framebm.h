#ifndef FRAMEBM_H
#define FRAMEBM_H

#include <QFrame>

namespace Ui {
class FrameBM;
}

class FrameBM : public QFrame
{
    Q_OBJECT

public:
    explicit FrameBM(QWidget *parent = 0);
    ~FrameBM();

    void setRsrv(bool s) { rsrv = s; }              // установить признак резерва
    void setSt  (class Station * pst)               // установить актуальную станцию
    {
        st = pst;
    }
    void Show(class Station * pst);                 // показать актуальную информацию по станции

private:
    Ui::FrameBM *ui;
    class Station * st;                             // актуальная станция
    bool  rsrv;                                     // признак того, что фрейм отображает резервный БМ
};

#endif // FRAMEBM_H
