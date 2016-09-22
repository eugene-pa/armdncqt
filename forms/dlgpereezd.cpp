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
}

DlgPereezd::~DlgPereezd()
{
    delete ui;
}
