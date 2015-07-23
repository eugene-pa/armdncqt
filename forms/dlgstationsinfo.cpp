#include "dlgstationsinfo.h"
#include "ui_dlgstationsinfo.h"
#include "../spr/station.h"

DlgStationsInfo::DlgStationsInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStationsInfo)
{
    ui->setupUi(this);

    QTableWidget * t = ui->tableStations;
    t->setColumnCount(8);
    t->setRowCount(Station::Stations.count());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Станция   " << " # " << " Ras " << "Адрес" << " Версия " << "ОТУ" << "КП ID" << "Конфигурация" );

    int row = 0;
    foreach (Station *st, Station::Stations.values())
    {
        t->setItem(row,0, new QTableWidgetItem (st->Name()));
        t->setItem(row,1, new QTableWidgetItem (QString("%01").arg(st->No(),2,10,QChar('0')))); // форматирование с ведущими нулями для сортировки
        t->setItem(row,2, new QTableWidgetItem (QString::number(st->Ras())));
        t->setItem(row,3, new QTableWidgetItem (QString::number(st->Addr())));
        t->setItem(row,4, new QTableWidgetItem (QString::number(st->Version())));
        // t->setItem(row,5, new QTableWidgetItem (QString::number(st->Version()));   // ОТУ
        t->setItem(row,6, new QTableWidgetItem (QString::number(st->GidUralId())));
        t->setItem(row,7, new QTableWidgetItem (st->Config()));

        row++;
    }

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(1, Qt::SortOrder::AscendingOrder);  // сортировка по умолчанию по №
}

DlgStationsInfo::~DlgStationsInfo()
{
    delete ui;
}
