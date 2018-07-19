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
    void SetOtuLed(bool ok, bool blink);                        // установить статус индикатора ОТУ и мигание
    void BlinkOtu();                                            // мигнуть индикатором ОТУ (актуальным цветом)

signals:
    void SelectStation(class Station *);

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::kpframe *ui;
    class  Station * st;
    class MainWindow * mainWindow;
    QColor getColor(bool rsrv, bool com4);
};

#endif // KPFRAME_H
