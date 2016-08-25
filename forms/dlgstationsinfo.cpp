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
    t->setRowCount((int)Station::Stations.size());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Станция   " << " # " << " Ras " << "Адрес" << " Версия " << "ОТУ" << "КП ID" << "Конфигурация" );

    // заполнение таблицы информацией о станциях
    // сортировка выполняется автоматически
    // указатель на объект запоминается в ячейках первого столбца
    // таблица может иметь динамическую составляющую:
    // - значок состояния работоспособности станции
    // - основной / резервный
    // - состояние СПОК/БРОК
    // - режим (ДУ/РУ/АУ)
    // - модемы COM3/COM4
    // - обобщенная диагностика КП
    int row = 0;

    for (auto rec : Station::Stations)
    {
        Station * st = rec.second;

        t->setItem(row,0, new QTableWidgetItem (*g_green, st->Name()));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)st));    // запомним станцию
        // так можно извлечь:
        //Station * stfrom = (Station*)(t->item(i,0)->data(Qt::UserRole).value<void*>());
        t->setItem(row,1, new QTableWidgetItem (QString("%1").arg(st->No(),2,10,QChar(' ')))); // форматирование с ведущими пробелами для сортировки
        t->setItem(row,2, new QTableWidgetItem (QString::number(st->Ras())));
        t->setItem(row,3, new QTableWidgetItem (QString("%1").arg(st->Addr(),2,10,QChar(' ')))); // форматирование с ведущими пробелами для сортировки
        t->setItem(row,4, new QTableWidgetItem (QString::number(st->Version())));
        // t->setItem(row,5, new QTableWidgetItem (QString::number(st->Version()));   // ОТУ
        t->setItem(row,6, new QTableWidgetItem (QString::number(st->GidUralId())));
        t->setItem(row,7, new QTableWidgetItem (st->Config()));

        row++;
    }

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(1, Qt::AscendingOrder);             // сортировка по умолчанию по №
}

DlgStationsInfo::~DlgStationsInfo()
{
    delete ui;
}
