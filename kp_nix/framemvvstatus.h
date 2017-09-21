#ifndef FRAMEMVVSTATUS_H
#define FRAMEMVVSTATUS_H

#include <QGroupBox>

namespace Ui {
class FrameMvvStatus;
}

class FrameMvvStatus : public QGroupBox
{
    Q_OBJECT

public:
    explicit FrameMvvStatus(QWidget *parent = 0);
    ~FrameMvvStatus();

private:
    Ui::FrameMvvStatus *ui;
};

#endif // FRAMEMVVSTATUS_H
