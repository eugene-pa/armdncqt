#include "dlgrcinfo.h"
#include "ui_dlgrcinfo.h"
#include "../spr/station.h"

QBrush DlgRcInfo::brushBusy     = QBrush(QColor(255,128,128));
QBrush DlgRcInfo::brushBuzyZmk  = QBrush(QColor(255,128,255));

DlgRcInfo::DlgRcInfo(Station * st, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::DlgRcInfo)
{
    this->st = st;
    ui->setupUi(this);

    fillData();
    startTimer (1500);
}

DlgRcInfo::~DlgRcInfo()
{
    ui->tableRc->clear();
    delete ui;
}

// смена станции
void DlgRcInfo::changeStation(class Station * p)
{
    if (p!=st)
    {
        ui->tableRc->clear();
        st = p;
        fillData();
    }
}

void DlgRcInfo::timerEvent(QTimerEvent *event)
{
    if (isVisible())
        updateStatus();
}

void DlgRcInfo::updateStatus()
{
    QTableWidget * t = ui->tableRc;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        Rc * rc = (Rc *) item->data(Qt::UserRole).value<void*>();
        if (rc==nullptr)
            continue;
        QBrush brush = getBackground(rc);
        if (brush != item->background())
            item->setBackground(getBackground(rc));
    }
}

// заполнение таблицы информацией о станциях
// сортировка выполняется автоматически
// указатель на объект запоминается в ячейках первого столбца
// динамическbt поля:
// - состояние РЦ (цвет)
// - маршрут
// - поезд
void DlgRcInfo::fillData()
{
    setWindowTitle("Состояние РЦ по ст." + st->Name());

    QTableWidget * t = ui->tableRc;
    t->setSortingEnabled(false);                             // запрещаем сортировку
    t->setColumnCount(10);
    t->setRowCount(st->Allrc().count());
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "Имя РЦ" << "#" << "Маршрут" << "Поезд" << "ТС" << "ТУ" << "Слева" << "Справа" << "Свтф >>" << "Свтф <<");

    t->setSelectionBehavior( QAbstractItemView::SelectRows );
    t->setSelectionMode(QAbstractItemView::SingleSelection);

    int row = 0;
    foreach (Rc *rc, st->Allrc().values())
    {
        // РЦ
        t->setItem(row,0, new QTableWidgetItem (/**g_green,*/ rc->Name()));
        t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)rc));    // запомним РЦ
        t->item(row,0)->setBackground(getBackground(rc));

        // #
        t->setItem(row,1, new QTableWidgetItem (QString("%1").arg(rc->No(),5,10,QChar(' ')))); // форматирование с ведущими нулями для сортировки

        // маршрут
        Route * route = rc->ActualRoute();
        t->setItem(row,2, new QTableWidgetItem ( route != nullptr ? route->Name() : ""));

        // поезд
        Train * train = rc->Actualtrain();
        t->setItem(row,3, new QTableWidgetItem ( train != nullptr ? train->Name() : ""));

        // ТС
        QString s;
        foreach (Ts *ts, rc->Allts())
        {
            s += ts->Name() + "   ";
        }
        t->setItem(row,4, new QTableWidgetItem (s));

        // ТУ
        s.clear();
        foreach (Tu *tu, rc->Alltu())
        {
            s += tu->Name() + "   ";
        }
        t->setItem(row,5, new QTableWidgetItem (s));

        // Слева
        s.clear();
        foreach (NxtPrv * lnk, rc->prv)
        {
            s += lnk->lft->Name();
            if (lnk->strl.count())
            {
                s += "[";
                foreach (LinkedStrl * lstrl, lnk->strl)
                {
                    s += lstrl->Name();
                    s += " ";
                }

                s += "]";
            }
            s += "  ";
        }
        t->setItem(row,6, new QTableWidgetItem (s));


        // Справа
        s.clear();
        foreach (NxtPrv * lnk, rc->nxt)
        {
            s += lnk->rht->Name();
            if (lnk->strl.count())
            {
                s += "[";
                foreach (LinkedStrl * lstrl, lnk->strl)
                {
                    s += lstrl->Name();
                    s += " ";
                }

                s += "]";
            }
            s += "  ";
        }
        t->setItem(row,7, new QTableWidgetItem (s));

        row++;
    }

    // "Свтф >>"

    // "Свтф <<"

    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::SortOrder::AscendingOrder);      // сортировка по умолчанию
    t->resizeColumnsToContents();
}

// при закрытии окна меняем видимость
void DlgRcInfo::closeEvent(QCloseEvent * e)
{
    setVisible(false);
    e->ignore();
}

QBrush DlgRcInfo::getBackground(Rc * rc)
{
    return rc->stsBusy ? rc->stsZmk ? brushBuzyZmk : brushBusy :
           rc->stsZmk  ? Qt::cyan : rc->actualRoute != nullptr ? Qt::green : Qt::white;

}
