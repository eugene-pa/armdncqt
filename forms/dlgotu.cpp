#include "dlgotu.h"
#include "ui_dlgotu.h"

DlgOtu::DlgOtu(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgOtu)
{
    ui->setupUi(this);

    QTableWidget * t = ui->tableOtu;                     // заполнение таблицы описания ТY

    t->setSelectionMode(QAbstractItemView::ContiguousSelection);
    t->setColumnCount(7);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "ОТУ" << "Код(#)" << "Реализация" << "ТС1" << "ТС2" << "ТУ эпилога" << "Сопряженная ОТУ");

    t->setSelectionBehavior( QAbstractItemView::SelectRows );
    t->setSelectionMode(QAbstractItemView::SingleSelection);

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
    QTableWidget * t = ui->tableOtu;

    t->clearContents();                                     // не обязательно, ресурсы освобождаются автоматически
    t->setSortingEnabled(false);
    t->setRowCount(pSt->Otu.count());

    int row = 0;
    foreach (Otu * otu, pSt->Otu.values())
    {
        // 0 - имя ТУ
        t->setItem(row,0, new QTableWidgetItem (otu->shortName));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)otu));// запомним ТУ
        //t->item(row,0)->setBackground(tu->delay<0.5 ? QBrush(QColor(224,224,224)) : Qt::white);

        row++;
    }
}
