#ifndef DLGTSINFO_H
#define DLGTSINFO_H

#include <QDialog>
#include "../spr/station.h"

namespace Ui {
class DlgTsInfo;
}

class DlgTsInfo : public QDialog
{
    Q_OBJECT

public slots:
    void changeStation(class Station *);                    // смена станции

private slots:
    void on_checkBox_toggled(bool checked);                 // клик нормализация
    void on_spinBox_valueChanged(int arg1);                 // смена страницы
    void on_tsSelected (int no);                            // выбор ТС

    void on_tableWidget_itemSelectionChanged();

public:
    explicit DlgTsInfo(QWidget *parent = 0, class Station * pst = nullptr);
    ~DlgTsInfo();


private:
    // события
    virtual void timerEvent(QTimerEvent *event);
    virtual void closeEvent(QCloseEvent *);

    QIcon getStsImage (class Ts * ts);                      // получить значок состояния ТС
    void UpdateList();                                      // обновить состояние ТС в списке
    Ui::DlgTsInfo *ui;
    class Station * pSt;


    void fillTable();
};

#endif // DLGTSINFO_H
