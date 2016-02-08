#ifndef DLGTUINFO_H
#define DLGTUINFO_H

#include <QDialog>
#include <../spr/station.h>

namespace Ui {
class DlgTuInfo;
}

class DlgTuInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTuInfo(QWidget *parent = 0, class Station * pst = nullptr);
    ~DlgTuInfo();

public slots:
    void changeStation(class Station *pst);

private slots:
    void on_tableTu_itemSelectionChanged();
    void on_SliderDelay_valueChanged(int value);

private:

    // события
    virtual void closeEvent(QCloseEvent *);

    void fillTable();                                       // заполнить таблицу имен ТС

    Ui::DlgTuInfo *ui;
    class Station * pSt;
};

#endif // DLGTUINFO_H
