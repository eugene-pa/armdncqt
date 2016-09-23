#include "QPainter"
#include "ui_dlgtsinfo.h"
#include "dlgtsinfo.h"
#include "tsstatuswidget.h"
#include "../spr/station.h"
#include "../spr/ts.h"

DlgTsInfo::DlgTsInfo(QWidget *parent, class Station * pst) :
    QDialog(parent),
    ui(new Ui::DlgTsInfo)
{
    ui->setupUi(this);

    st = nullptr;
    ui->spinBox->setRange(1,4);                             // страницы 1-4

    QTableWidget * t = ui->tableWidget;                     // заполнение таблицы описания ТС
    t->setSelectionMode(QAbstractItemView::ContiguousSelection);
    t->setColumnCount(4);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "№" << "Сигнал" << "Место" << "M/I/J");

    // автоматически растягтваем 2-й столбец
    t->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // привязываем обработчик выбора сигнала виджета widgetTs
    QObject::connect(ui->widgetTs, SIGNAL(tsSelected (int)), this, SLOT(on_tsSelected(int)));

    changeStation(pst);
    startTimer(1500);
}

DlgTsInfo::~DlgTsInfo()
{
    delete ui;
}

// слот обработки события смена станции
void DlgTsInfo::changeStation(class Station *pst)
{
    if (pst!=st && pst!=nullptr)
    {
        st = pst;
        ui->labelSt->setText(pst->Name());                      // имя станции
        ui->widgetTs->setNormal(ui->checkBox->isChecked());     // состояние нормализации

        fillTable();                                            // заполнить таблицу имен ТС
        ui->widgetTs->updateWidget(st = pst);                  // отрисовка ТС
    }
}

// получить значок состояния ТС
QIcon DlgTsInfo::getStsImage (Ts * ts)
{
    return QIcon(*(ts->StsPulse() ? g_green_box_blink : (ui->checkBox->isChecked() ? ts->Sts() : ts->StsRaw()) ? g_green_dark_box : g_white_box));
}


// заполнение таблицы ТС
void DlgTsInfo::fillTable()
{
    QTableWidget * t = ui->tableWidget;

    t->clearContents();                                     // не обязательно, ресурсы освобождаются автоматически
    t->setSortingEnabled(false);
    t->setRowCount((int)st->Ts.size());

    int row = 0;
    for (auto rec : st->Ts)
    {

        Ts * ts = rec.second;
        // 1 столбец - номер сигнала = индекс+1
        t->setItem(row,0, new QTableWidgetItem (getStsImage(ts), QString("%1").arg(ts->GetIndex() + 1,5,10,QChar(' '))));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)ts));// запомним ТС

        // 2 столбец - имя сигнала
        QString name = ts->Name();
        t->setItem(row,1, new QTableWidgetItem (name));     // имя

        t->setItem(row,2, new QTableWidgetItem (ts->Place()));
        t->setItem(row,3, new QTableWidgetItem (QString("%1/%2/%3").arg(ts->M()).arg(ts->I()).arg(ts->J())));

        QColor clr = ts->Disabled() ? Qt::gray : ts->IsVirtual() ? Qt::darkBlue : ts->IsPulsing () ? Qt::red : ts->IsInverse() ? Qt::white : Qt::black;
        if (clr != Qt::black)
            for (int i=0; i<4; i++)
            {
                t->item(row,i)->setForeground(clr);
                if (ts->IsInverse())
                    t->item(row,i)->setBackground(Qt::darkGray);
            }

        row++;
    }

    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::AscendingOrder);                 // сортировка по умолчанию
    t->resizeColumnsToContents();

}

// обработка событий таймера
void DlgTsInfo::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        UpdateList();
}

// обновить состояние ТС в списке
void DlgTsInfo::UpdateList()
{
    QTableWidget * t = ui->tableWidget;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        Ts * ts = (Ts *) item->data(Qt::UserRole).value<void*>();
        if (ts==nullptr)
            continue;
        item->setIcon(getStsImage(ts));
    }
}

// слот обработки флажка "Нормализация"
void DlgTsInfo::on_checkBox_toggled(bool checked)
{
    UpdateList();
    ui->widgetTs->setNormal(checked);
}

// слот обработки поля смены страницы
void DlgTsInfo::on_spinBox_valueChanged(int arg1)
{
    ui->widgetTs->updateWidget(st, arg1);
}

// слот обработки уведомления о выборе ТС
// учитывая произвольный характер сортировки, перебираем список вручную
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

// при закрытии окна меняем видимость
void DlgTsInfo::closeEvent(QCloseEvent * e)
{
    setVisible(false);
    e->ignore();
}

// обработка выбора ТС в списке
void DlgTsInfo::on_tableWidget_itemSelectionChanged()
{
    QTableWidgetItem * item = ui->tableWidget->selectedItems()[0];
    Ts * ts = (Ts *) ui->tableWidget->item(item->row(),0)->data(Qt::UserRole).value<void*>();
    if (ts != nullptr)
    {
        ui->spinBox->setValue(ts->GetIndex() / 1024 + 1);       // сменить страницу, если надо
        ui->widgetTs->setActualNode(ts->GetIndex() % 1024);     // выделить актуальный сигнал
    }
}
