#include "dlgotu.h"
#include "ui_dlgotu.h"

DlgOtu::DlgOtu(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgOtu)
{
    ui->setupUi(this);

    QTableWidget * t = ui->tableOtu;                     // заполнение таблицы описания ТY

    t->setSelectionMode(QAbstractItemView::ContiguousSelection);
    t->setColumnCount(8);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "ОТУ" << "Код(#)" << "Реализация" << "ТС1" << "ТС2" << "ТУ эпилога" << "Сопряженная ОТУ" << "ЭЦ");

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
    if (pst!=pSt && pst!=nullptr)
    {
        pSt = pst;
        setWindowTitle("Список ОТУ по станции " + pst->Name());
        fillTable();                                            // заполнить таблицу имен ТС
    }
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
    t->setRowCount((int)pSt->Otu.size());

    int row = 0;
    for (auto rec : pSt->OtuByNo)
    {
        Otu * otu = rec.second;

        // 0 - имя ОТУ
        t->setItem(row,0, new QTableWidgetItem (otu->shortName));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)otu));// запомним ТУ
        //t->item(row,0)->setBackground(tu->delay<0.5 ? QBrush(QColor(224,224,224)) : Qt::white);

        // 1 - Код (#)
        t->setItem(row,1, new QTableWidgetItem (QString::number(otu->no)));

        // 2 - БРОК/СПО
        t->setItem(row,2, new QTableWidgetItem (otu->isUpok() ? "БРОК" : "СПОК"));

        // 3 - ТС1
        t->setItem(row,3, new QTableWidgetItem (otu->nameTs1));

        // 4 - ТС2
        t->setItem(row,4, new QTableWidgetItem (otu->nameTs2));

        // 5 - ТУ эпилог
        t->setItem(row,5, new QTableWidgetItem (otu->epilogTu));

        // 6 - Сопряженная ОТУ
        if (otu->no2)
            t->setItem(row,6, new QTableWidgetItem (otu->st->OtuByNo[otu->no2]->ToString()));

        // 7 - Тип ЭЦ (алгоритма)
        QString s = otu->TypeOfObject().length() ? otu->TypeOfObject() : otu->st->TypeEC();
        t->setItem(row,7, new QTableWidgetItem (s));

        row++;
    }

    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(1, Qt::AscendingOrder);                 // сортировка по умолчанию
    t->resizeColumnsToContents();

}
