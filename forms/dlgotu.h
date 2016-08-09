#ifndef DLGOTU_H
#define DLGOTU_H

#include <QDialog>
#include <../spr/station.h>

namespace Ui {
class DlgOtu;
}

class DlgOtu : public QDialog
{
    Q_OBJECT

public:
    explicit DlgOtu(QWidget *parent = 0, class Station * pst = nullptr);
    ~DlgOtu();

public slots:
    void changeStation(class Station *pst);

private:
    // события
    virtual void closeEvent(QCloseEvent *);
    void fillTable();                                       // заполнить таблицу имен ТС

    // закрытые члены
    class Station * pSt;
    Ui::DlgOtu *ui;
};

#endif // DLGOTU_H
