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

private:
    Ui::FrameMVV *ui;
};

#endif // FRAMEMVV_H
