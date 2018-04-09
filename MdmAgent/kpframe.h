#ifndef KPFRAME_H
#define KPFRAME_H

#include <QFrame>
#include "../spr/station.h"

namespace Ui {
class kpframe;
}

class kpframe : public QFrame
{
    Q_OBJECT

public:
    explicit kpframe(QWidget *parent, class Station * pst);
    ~kpframe();

    void SetActual(bool s, bool rsrv);

private:
    Ui::kpframe *ui;
    class  Station * st;
};

#endif // KPFRAME_H
