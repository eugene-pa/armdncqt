#include "dlgpereezd.h"
#include "ui_dlgpereezd.h"

DlgPereezd::DlgPereezd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPereezd)
{
    ui->setupUi(this);
    QTableWidget * t = ui->table;
    t->setColumnCount(10);
    t->setRowCount((int)Pereezd::Pereezds.size());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "#" << "Станция" << "Переезд" << "Открыт" << "Авария" << "Заград.сгн" << "Извещение" << "Перегон" << "Горловина" << "РЦ" );

    // автоматически растягтваем 1-й столбец
    t->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Stretch);

    int row = 0;

    for (auto rec : Pereezd::Pereezds)
    {
        Pereezd * p = rec.second;

        t->setItem(row,0, new QTableWidgetItem (QString::number(p->No())));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)p));     // запомним переезд

        if (p->st!=nullptr)
            t->setItem(row,1, new QTableWidgetItem (p->st->Name()));            // станция

        t->setItem(row,2, new QTableWidgetItem (p->name));                      // наименование

        if (p->openVal != nullptr)
            t->setItem(row,3, new QTableWidgetItem (p->openVal->Source()));     // открыт
        if (p->alarmVal != nullptr)
            t->setItem(row,4, new QTableWidgetItem (p->alarmVal->Source()));    // авария
        if (p->zagrVal != nullptr)
            t->setItem(row,5, new QTableWidgetItem (p->zagrVal->Source()));     // заград.сигнализация
        if (p->izvVal != nullptr)
            t->setItem(row,6, new QTableWidgetItem (p->izvVal->Source()));      // извещение

        if (p->prg!=nullptr)
            t->setItem(row,7, new QTableWidgetItem (p->prg->Name()));           // перегон

        t->setItem(row,8, new QTableWidgetItem (p->side ? "Н" : "Ч"));          // горловина
        t->setItem(row,9, new QTableWidgetItem (p->srcRc));                     // РЦ

        row++;
    }

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                                                 // разрешаем сортировку
    t->sortByColumn(2, Qt::AscendingOrder);                                     // сортировка по умолчанию по №

    updateStatus();
    startTimer (1500);
}

DlgPereezd::~DlgPereezd()
{
    delete ui;
}

void DlgPereezd::timerEvent(QTimerEvent *)
{
    updateStatus();
}

void DlgPereezd::updateStatus()
{
    QTableWidget * t = ui->table;
    for (int row=0; row<t->rowCount(); row++)
    {
        QTableWidgetItem * item = t->item(row,0);
        Pereezd * p = (Pereezd *) item->data(Qt::UserRole).value<void*>();
        if (p==nullptr)
            continue;
        if (p->openVal)
            t->item(row,3)->setForeground(p->openVal->GetValue() ? Qt::darkGreen : Qt::darkRed);
        if (p->alarmVal)
        {
            t->item(row,4)->setBackground(p->alarmVal->GetValue() ? Qt::yellow  : Qt::white);
            t->item(row,4)->setForeground(p->alarmVal->GetValue() ? Qt::darkRed : Qt::black);
        }
        if (p->zagrVal)
            t->item(row,5)->setForeground(p->zagrVal->GetValue() ? Qt::red : Qt::black);
        if (p->izvVal)
            t->item(row,6)->setForeground(p->izvVal->GetValue() ? Qt::darkRed : Qt::black);

    }
}

void DlgPereezd::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}

