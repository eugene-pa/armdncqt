#include "dlgtuinfo.h"
#include "ui_dlgtuinfo.h"

DlgTuInfo::DlgTuInfo(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgTuInfo)
{
    ui->setupUi(this);

    QTableWidget * t = ui->tableTu;                         // заполнение таблицы описания ТY

    t->setSelectionMode(QAbstractItemView::ContiguousSelection);
    t->setColumnCount(8);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "ТУ" << "М / I / J" << "Колодка" << "Контакт" << "Время" << "Полюс" << "Пролог" << "Эпилог");

    t->setSelectionBehavior( QAbstractItemView::SelectRows );
    t->setSelectionMode(QAbstractItemView::SingleSelection);

    changeStation(pst);
}

DlgTuInfo::~DlgTuInfo()
{
    delete ui;
}

// слот обработки события смена станции
void DlgTuInfo::changeStation(Station *pst)
{
    if (pSt == pst || pst==nullptr)
        return;

    pSt = pst;
    setWindowTitle("ТУ по станции " + pst->Name());

    ui->SliderDelay->setMaximum(pst->Kp2000() ? 6 : 15);

    fillTable();                                            // заполнить таблицу имен ТС
}

void DlgTuInfo::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}

// заполнить таблицу имен ТС
void DlgTuInfo::fillTable()
{
    QTableWidget * t = ui->tableTu;

    t->clearContents();                                     // не обязательно, ресурсы освобождаются автоматически
    t->setSortingEnabled(false);
    t->setRowCount((int)pSt->Tu.size());

    int row = 0;
    for (auto rec : pSt->Tu)
    {
        class Tu * tu = rec.second;

        // 0 - имя ТУ
        t->setItem(row,0, new QTableWidgetItem (QString("%1    %2").arg(tu->Name()).arg(tu->extTuSrc)));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)tu));// запомним ТУ
        t->item(row,0)->setBackground(tu->delay<0.5 ? QBrush(QColor(224,224,224)) : Qt::white);

        // 1 - М / I / J
        t->setItem(row,1, new QTableWidgetItem (QString("%1 %2 %3").arg(tu->modul,2,10,QChar(' ')).arg(tu->_i,5,10,QChar(' ')).arg(tu->_j,5,10,QChar(' '))));

        // 2 - Колодка
        t->setItem(row,2, new QTableWidgetItem (tu->kolodka));

        // 3 - Контакт
        //     TODO:  ДОРАБОТАТЬ ДЛЯ КП-2007 С УЧЕТОМ ТАБЛИЦЫ КЛЕММ
        t->setItem(row,3, new QTableWidgetItem (tu->kontact));

        // 4 - Время
        t->setItem(row,4, new QTableWidgetItem (QString("%1").arg(tu->delay)));

        // 5 - Полюс
        if (tu->tuPolus)
            t->setItem(row,5, new QTableWidgetItem (tu->tuPolus->Name()));

        // 6 - Пролог
        if (tu->tuProlog)
            t->setItem(row,6, new QTableWidgetItem (tu->tuProlog->Name()));

        // 7 - Эпилог
        if (tu->tuEpilog)
            t->setItem(row,7, new QTableWidgetItem (tu->tuEpilog->Name()));

        row++;
    }


    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::AscendingOrder);                 // сортировка по умолчанию
    t->resizeColumnsToContents();
}


void DlgTuInfo::on_tableTu_itemSelectionChanged()
{
    Tu * tu = (Tu *) ui->tableTu->item(ui->tableTu->currentRow(), 0)->data(Qt::UserRole).value<void*>();
    if (tu != nullptr)
    {
        ui->lineEditDelay->setText(QString("%1").arg(tu->delay));
        ui->SliderDelay->setValue((int)tu->delay);
    }
}

void DlgTuInfo::on_SliderDelay_valueChanged(int value)
{
    Q_UNUSED(value)
    ui->lineEditDelay->setText(QString("%1").arg(ui->SliderDelay->value()));
}
