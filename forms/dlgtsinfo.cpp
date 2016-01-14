#include "QPainter"

#include "dlgtsinfo.h"
#include "tsstatuswidget.h"
#include "ui_dlgtsinfo.h"
#include "../spr/station.h"
#include "../spr/ts.h"

DlgTsInfo::DlgTsInfo(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgTsInfo)
{
    ui->setupUi(this);
    ui->labelSt->setText(pst->Name());

    ui->widgetTs->updateWidget(pSt = pst);                  // отрисовка ТС
    ui->widgetTs->setNormal(ui->checkBox->isChecked());

    QTableWidget * t = ui->tableWidget;                     // заполнение таблицы описания ТС
    t->setColumnCount(4);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "№" << "Сигнал" << "Место" << "M/I/J");
    fillTable();

    startTimer(1000);
}

DlgTsInfo::~DlgTsInfo()
{
    delete ui;
}

void DlgTsInfo::fillTable()
{
    QTableWidget * t = ui->tableWidget;
    t->setRowCount(pSt->TsIndexed.count());

    int row = 0;
    foreach (Ts * ts, pSt->TsIndexed.values())
    {
        QPixmap * img = ts->StsPulse() ? g_green_box_blink : ts->Sts() ? g_green_dark_box : g_white_box;
        t->setItem(row,0, new QTableWidgetItem (*img, QString("%1").arg(ts->GetIndex(),5,10,QChar(' ')))); // №
                            //QTableWidgetItem (QString("%1").arg(rc->No()      ,5,10,QChar(' ')))); // форматирование с ведущими нулями для сортировки
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)ts));                    // запомним ТС

        t->setItem(row,1, new QTableWidgetItem (ts->Name()));                                    // имя

        t->setItem(row,2, new QTableWidgetItem (QString("%1:%2").arg(ts->Kolodka()).arg(ts->Kontact())));
        t->setItem(row,3, new QTableWidgetItem (QString("%1/%2/%3").arg(ts->M()).arg(ts->I()).arg(ts->J())));
        row++;
    }

    t->setSortingEnabled(true);                                                                 // разрешаем сортировку
    t->sortByColumn(0, Qt::SortOrder::AscendingOrder);                                          // сортировка по умолчанию
    t->resizeColumnsToContents();

}

void DlgTsInfo::paintEvent(QPaintEvent *)
{
//    QPainter p(this);                       // Создаём новый объект рисовальщика
//    p.setPen(QPen(Qt::red,1,Qt::SolidLine));    // Настройки рисования
//    p.drawLine(0,0,100,100);           // Рисование линии
}

void DlgTsInfo::timerEvent(QTimerEvent *event)
{
    QTableWidget * t = ui->tableWidget;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        QVariant p = item->data(Qt::UserRole);
        Ts * ts = (Ts *) item->data(Qt::UserRole).value<void*>();
        QPixmap * img = ts->StsPulse() ? g_green_box_blink : ts->Sts() ? g_green_dark_box : g_white_box;
        item->setIcon(QIcon(*img));
        int a = 99;
    }
}

void DlgTsInfo::on_checkBox_toggled(bool checked)
{
    ui->widgetTs->setNormal(checked);
}
