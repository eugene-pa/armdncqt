#include "QPainter"
#include "QCloseEvent"

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

    ChangeStation(pst);

    QObject::connect(ui->widgetTs, SIGNAL(tsSelected (int)), this, SLOT(on_tsSelected(int)));
    startTimer(1000);
}

DlgTsInfo::~DlgTsInfo()
{
    delete ui;
}

void DlgTsInfo::ChangeStation(class Station *pst)
{
    pSt = pst;
    ui->labelSt->setText(pst->Name());

    ui->widgetTs->setNormal(ui->checkBox->isChecked());
    ui->spinBox->setRange(1,4);

    QTableWidget * t = ui->tableWidget;                     // заполнение таблицы описания ТС
    t->clear();

    t->setColumnCount(4);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "№" << "Сигнал" << "Место" << "M/I/J");
    fillTable();

    ui->widgetTs->updateWidget(pSt = pst);                  // отрисовка ТС
}

void DlgTsInfo::fillTable()
{
    QTableWidget * t = ui->tableWidget;
    t->setRowCount(pSt->Ts.count());

    int row = 0;
    foreach (Ts * ts, pSt->Ts.values())
    {
        QPixmap * img = ts->StsPulse() ? g_green_box_blink : ts->Sts() ? g_green_dark_box : g_white_box;
        // 1 столбец - номер сигнала = индекс+1
        t->setItem(row,0, new QTableWidgetItem (*img, QString("%1").arg(ts->GetIndex() + 1,5,10,QChar(' '))));

        // 2 столбец - имя сигнала
        QString name = ts->Name();
        t->setItem(row,1, new QTableWidgetItem (name));                                    // имя
        //t->setItem(row,1, new QTableWidgetItem (ts->Name()));                                    // имя

        t->setItem(row,2, new QTableWidgetItem (QString("%1:%2").arg(ts->Kolodka()).arg(ts->Kontact())));
        t->setItem(row,3, new QTableWidgetItem (QString("%1/%2/%3").arg(ts->M()).arg(ts->I()).arg(ts->J())));

        QColor clr = ts->Locked() ? Qt::gray : ts->IsVirtual() ? Qt::darkBlue : ts->IsPulsing () ? Qt::red : ts->IsInverse() ? Qt::white : Qt::black;
        if (clr != Qt::black)
            for (int i=0; i<4; i++)
            {
                t->item(row,i)->setForeground(clr);
                if (ts->IsInverse())
                    t->item(row,i)->setBackground(Qt::darkGray);
            }

        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)ts));                    // запомним ТС

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
        Ts * ts = (Ts *) item->data(Qt::UserRole).value<void*>();
        if (ts==nullptr)
            return;
        QPixmap * img = ts->StsPulse() ? g_green_box_blink : ts->Sts() ? g_green_dark_box : g_white_box;
        item->setIcon(QIcon(*img));
    }
}

void DlgTsInfo::on_checkBox_toggled(bool checked)
{
    ui->widgetTs->setNormal(checked);
}

void DlgTsInfo::on_spinBox_valueChanged(int arg1)
{
    ui->widgetTs->updateWidget(pSt, arg1);
}

// слот обработки уведомления о выборе ТС
void DlgTsInfo::on_tsSelected (int no)
{
    QTableWidget * t = ui->tableWidget;
    for(int i=0; i<t->rowCount(); i++)
    {
        Ts * ts = (Ts *) t->item(i,0)->data(Qt::UserRole).value<void*>();
        if (ts->GetIndex() + 1 == no)
        {
            t->selectRow(i);
            break;
        }
    }
}

void DlgTsInfo::closeEvent(QCloseEvent * e)
{
    setVisible(false);
    e->ignore();
}
