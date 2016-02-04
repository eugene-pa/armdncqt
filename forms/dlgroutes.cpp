#include "dlgroutes.h"
#include "ui_dlgroutes.h"
#include "../spr/station.h"

DlgRoutes::DlgRoutes(class Station * st, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRoutes)
{
    this->st = st;
    ui->setupUi(this);

    FillData();

}

DlgRoutes::~DlgRoutes()
{
    delete ui;
}

// смена станции
void DlgRoutes::changeStation(class Station * p)
{
    if (p!=st)
    {
        ui->tableRoutes->clear();
        st = p;
        FillData();
    }
}

// при закрытии окна меняем видимость
void DlgRoutes::closeEvent(QCloseEvent * e)
{
    setVisible(false);
    e->ignore();
}


// заполнение таблицы информацией о станциях
// сортировка выполняется автоматически
// указатель на объект запоминается в ячейках первого столбца
// динамическbt поля:
// - состояние РЦ (цвет)
// - маршрут
// - поезд
void DlgRoutes::FillData()
{
    setWindowTitle("Маршруты по ст." + st->Name());

    QTableWidget * t = ui->tableRoutes;
    t->setSortingEnabled(false);                             // запрещаем сортировку
    t->setColumnCount(13);
    t->setRowCount(st->Allroute().count());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "#" << "Наименование" << "Тип" << "Светофор" << "РЦ в маршруте" << "Стрелки в маршруте" << "ТУ установки" << "ТУ отмены" << "Замыкание" << "Набор" << "Отмена набора" << "ТС перегон" << "ТУ разворот" );

    int row = 0;

    foreach (Route *route, st->Allroute().values())
    {
        // номер (форматируем с ведущими пробелами)
        t->setItem(row,0, new QTableWidgetItem (QString("%1").arg(route->RelNo(),5,10,QChar(' '))));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)route));    // запомним маршрут

        // Наименование
        t->setItem(row,1, new QTableWidgetItem (*g_white, route->Text()));

        // тип
        QString temp = route->TypeName();
        if (route->IsComplex())
        {
            temp += "=" + route->srcComplex;
        }
        t->setItem(row,2, new QTableWidgetItem (temp));
        if (route->srcComplexError)
            t->item(row,2)->setForeground(Qt::red);

        // светофор
        t->setItem(row,3, new QTableWidgetItem (route->SvtfBeg()==nullptr ? "" : route->SvtfBeg()->Name()));

        // РЦ в маршруте
        t->setItem(row,4, new QTableWidgetItem (route->srcRc));
        if (route->srcRcError)
        {
            t->item(row,4)->setForeground(Qt::red);
        }

        row++;
    }


    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::SortOrder::AscendingOrder);      // сортировка по умолчанию
    t->resizeColumnsToContents();
}
