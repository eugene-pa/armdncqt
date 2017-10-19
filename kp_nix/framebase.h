#ifndef FRAMEBASE_H
#define FRAMEBASE_H

#include <QFrame>

namespace Ui {
class FrameBase;
}

class FrameBase : public QFrame
{
    Q_OBJECT

public:
    explicit FrameBase(QWidget *parent = 0);
    ~FrameBase();

private:
    Ui::FrameBase *ui;
};

#endif // FRAMEBASE_H
