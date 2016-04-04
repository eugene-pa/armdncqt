#ifndef KPFRAME_H
#define KPFRAME_H

#include <QFrame>

namespace Ui {
class kpframe;
}

class kpframe : public QFrame
{
    Q_OBJECT

public:
    explicit kpframe(QWidget *parent = 0);
    ~kpframe();

private:
    Ui::kpframe *ui;
};

#endif // KPFRAME_H
