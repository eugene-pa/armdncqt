#include "dlgtrains.h"
#include "../spr/train.h"
#include "../spr/rc.h"
#include "ui_dlgtrains.h"

DlgTrains::DlgTrains(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTrains)
{
    ui->setupUi(this);
    QTableWidget * t = ui->tableTrains;
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setColumnCount(3);
    t->setHorizontalHeaderLabels(QStringList() << "sno" << "No" << "РЦ" );
    fill(ui->checkBox->isChecked());

    startTimer(1000);
}

DlgTrains::~DlgTrains()
{
    delete ui;
}

// добавить в таблицу инфу по поезду
void DlgTrains::addTrainInfo(int row, Train * train)
{
    QTableWidget * t = ui->tableTrains;

    t->setItem(row,0, new QTableWidgetItem (QString::number(train->sno)));      // sno
    t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)train));     // запомним справочник

    t->setItem(row,1, new QTableWidgetItem (QString::number(train->no)));       // no

    QString rc;
    for (int i=0; i<train->nrc; i++)
    {
        rc += QString("%1  ").arg(train->Rc[i]->NameEx());
    }

    t->setItem(row,2, new QTableWidgetItem (rc));                               // РЦ
    train->marked = true;
}

void DlgTrains::fill(bool all)
{
    QTableWidget * t = ui->tableTrains;
    t->clearContents();
    t->setRowCount((int)Train::Trains.size());
    t->setSortingEnabled(false);

    int row = 0;
    for (auto rec : Train::Trains)
    {
        Train * train = rec.second;
        if (!all && train->no==0)
            continue;

        addTrainInfo(row++, train);
    }
    t->setRowCount(row);

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(1, Qt::AscendingOrder);             // сортировка по умолчанию по №

}

void DlgTrains::on_checkBox_toggled(bool checked)
{
    fill(checked);
}

void DlgTrains::updateList()
{
    Train::ClearMark();                                 // очищаем пометки

    QTableWidget * t = ui->tableTrains;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        Train * train = (Train *) item->data(Qt::UserRole).value<void*>();
        if (train==nullptr)
            continue;

        // надо бы проверить актуальность поезда по времени
        addTrainInfo(i, train);
    }

    // добавить не помеченные поезда (которых еще нет в списке)
    int row = t->rowCount();
    for (auto rec : Train::Trains)
    {
        Train * train = rec.second;
        if (train->marked || (!ui->checkBox->isChecked() && train->no==0))
            continue;
        // если поезда нет в списке - добавляем
        addTrainInfo(row++, train);
    }

}

void DlgTrains::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        updateList();
}
