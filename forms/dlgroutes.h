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

public slots:
    void changeStation(class Station *);                    // смена станции

private slots:

private:
    virtual void closeEvent(QCloseEvent *);
    void FillData();
    class Station * st;

    Ui::DlgRoutes *ui;
};

#endif // DLGROUTES_H
