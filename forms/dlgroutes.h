#ifndef DLGROUTES_H
#define DLGROUTES_H

#include <QDialog>

namespace Ui {
class DlgRoutes;
}

class DlgRoutes : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRoutes(class Station * st, QWidget *parent = 0);
    ~DlgRoutes();

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void closeEvent(QCloseEvent *);

public slots:
    void changeStation(class Station *);                    // смена станции

private slots:

private:
    void FillData();
    QIcon getStsImage (class Route * route);                // получить значок состояния ТС

    class Station * st;
    Ui::DlgRoutes *ui;
};

#endif // DLGROUTES_H
