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
    void SelectSt(bool s);

//public slots:
    void Show();
    void SetActual(bool s, bool rsrv);

signals:
    void SelectStation(class Station *);

private slots:
    void on_pushButton_clicked();

private:
    Ui::kpframe *ui;
    class  Station * st;
    class MainWindow * mainWindow;
    QColor getColor(bool rsrv, bool com4);
};

#endif // KPFRAME_H
