#include "dlgperegoninfo.h"
#include "ui_dlgperegoninfo.h"
#include "../spr/peregon.h"

DlgPeregonInfo::DlgPeregonInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPeregonInfo)
{
    ui->setupUi(this);

    QTableWidget * t = ui->table;
    t->setColumnCount(11);
    t->setRowCount((int)Peregon::Peregons.size());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Перегон" << "Путей" << "Слепой" << "Занятость ЧЕТ" << "Занятость НЕЧЕТ" << "Направл.1" << "Направл.2" << "tЧ" << "tН"<< "Поезда ЧЕТ" << "Поезда НЕЧЕТ" );

    // автоматически растягтваем 1-й столбец
    t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    int row = 0;

    for (auto rec : Peregon::Peregons)
    {
        Peregon * p = rec.second;

        t->setItem(row,0, new QTableWidgetItem ("#" + QString("%1").arg(p->No(),2,10,QChar(' ')) + " - " + p->Name()));



        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)p));     // запомним перегон

        t->setItem(row,1, new QTableWidgetItem (QString::number(p->pathes)));   // путей
        t->setItem(row,2, new QTableWidgetItem (p->blind ? "ДА" : ""));         // слепой

        if (p->busyEvnExpr != nullptr)
            t->setItem(row,3, new QTableWidgetItem (p->busyEvnExpr->Source()));// занятость в четном направлении
        if (p->busyOddExpr != nullptr)
            t->setItem(row,4, new QTableWidgetItem (p->busyOddExpr->Source()));// занятость в четном направлении

        t->setItem(row,5, new QTableWidgetItem (p->dir1Odd));                   // неч.направление по ТС левой  станции (логич.контроль)
        t->setItem(row,6, new QTableWidgetItem (p->dir2Odd));                   // неч.направление по ТС правой станции (логич.контроль)
        t->setItem(row,7, new QTableWidgetItem (QString::number(p->blindEvnTime)));// время хода в четном направлении
        t->setItem(row,8, new QTableWidgetItem (QString::number(p->blindOddTime)));// время хода в четном направлении

        t->setItem(row,9, new QTableWidgetItem (""));                           // четные поезда
        t->setItem(row,10, new QTableWidgetItem (""));                          // нечетные поезда

        row++;
    }

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(0, Qt::AscendingOrder);             // сортировка по умолчанию по №

    updateStatus();

    startTimer (1500);
}

void DlgPeregonInfo::updateStatus()
{
    QTableWidget * t = ui->table;
    for (int row=0; row<t->rowCount(); row++)
    {
        QTableWidgetItem * item = t->item(row,0);
        Peregon * p = (Peregon *) item->data(Qt::UserRole).value<void*>();
        if (p==nullptr)
            continue;
        if (p->busyEvnExpr != nullptr)
            t->item(row,3)->setForeground(p->busyEvnExpr->ValueBool() ? Qt::red : Qt::black);
        if (p->busyOddExpr != nullptr)
            t->item(row,4)->setForeground(p->busyOddExpr->ValueBool() ? Qt::red : Qt::black);

        s.clear();
        for (int i=0; i<p->evnTrains.size(); i++)
        {
            const Train * tr = p->evnTrains[i];
            s += QString::number(tr->sno) + "/" + QString::number(tr->no);
        }
        t->setItem(row,9, new QTableWidgetItem (s));                            // четные поезда

        s.clear();
        for (int i=0; i<p->oddTrains.size(); i++)
        {
            const Train * tr = p->oddTrains.at(i);
            s += QString::number(tr->sno) + "/" + QString::number(tr->no);
        }
        t->setItem(row,10, new QTableWidgetItem (s));                            // нечетные поезда

    }
}

DlgPeregonInfo::~DlgPeregonInfo()
{
    delete ui;
}

void DlgPeregonInfo::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        updateStatus();
}

void DlgPeregonInfo::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}
