#include "dlgotu.h"
#include "ui_dlgotu.h"

DlgOtu::DlgOtu(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgOtu)
{
    ui->setupUi(this);

    changeStation(pst);
}

DlgOtu::~DlgOtu()
{
    delete ui;
}

// слот обработки события смена станции
void DlgOtu::changeStation(Station *pst)
{
    if (pSt == pst || pst==nullptr)
        return;

    pSt = pst;
    setWindowTitle("Список ОТУ по станции " + pst->Name());

    fillTable();                                            // заполнить таблицу имен ТС
}


void DlgOtu::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}


// заполнить таблицу имен ТС
void DlgOtu::fillTable()
{

}
