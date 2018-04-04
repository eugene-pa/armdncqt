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

private:
    Ui::FrameBM *ui;
};

#endif // FRAMEBM_H
