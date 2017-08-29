#ifndef FRAMEBT_H
#define FRAMEBT_H

#include <QGroupBox>

namespace Ui {
class FrameBT;
}

class FrameBT : public QGroupBox
{
    Q_OBJECT

public:
    explicit FrameBT(QWidget *parent = 0);
    ~FrameBT();

private:
    Ui::FrameBT *ui;
};

#endif // FRAMEBT_H
