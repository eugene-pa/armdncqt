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

    startTimer (1500);
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
    int errors = 0;

    foreach (Route *route, st->Allroute().values())
    {
        // 0 - номер (форматируем с ведущими пробелами)
        t->setItem(row,0, new QTableWidgetItem (QString("%1").arg(route->RelNo(),5,10,QChar(' '))));

        // 1 - Наименование
        t->setItem(row,1, new QTableWidgetItem (/*getStsImage (route),*/route->Text()));
        t->item(row,1)->setData(Qt::UserRole,qVariantFromValue((void *)route));    // запомним маршрут
        t->item(row,1)->setBackground(getBrush(route));

        // 2 - тип
        QString temp = route->TypeName();
        if (route->IsComplex())
        {
            temp += "=" + route->srcComplex;
        }
        t->setItem(row,2, new QTableWidgetItem (temp));
        if (route->srcComplexError)
        {
            t->item(row,2)->setForeground(Qt::red);
            t->item(row,2)->setBackground(Qt::yellow);
            errors++;
        }

        // 3 - светофор
        temp = route->SvtfBeg()==nullptr ? "" : route->SvtfBeg()->Name();
        if (route->svtfEnd != nullptr)
          temp += " -> " + route->svtfEnd->Name();
        t->setItem(row,3, new QTableWidgetItem (temp));

        // 4 - РЦ в маршруте
        t->setItem(row,4, new QTableWidgetItem (route->srcRc));
        if (route->srcRcError)
        {
            t->item(row,4)->setText(QString("%1  ОК= %2").arg(t->item(row,4)->text()).arg(route->GetRcEnum()));
            t->item(row,4)->setForeground(Qt::red);
            t->item(row,4)->setBackground(Qt::yellow);
            errors++;
        }

        // 5 - стрелки в маршруте
        t->setItem(row,5, new QTableWidgetItem (route->srcStrl));
        if (route->srcStrlError)
        {
            t->item(row,5)->setText(QString("%1  ОК= %2").arg(t->item(row,5)->text()).arg(route->GetStrlEnum()));
            t->item(row,5)->setForeground(Qt::red);
            t->item(row,5)->setBackground(Qt::yellow);
            errors++;
        }

        // 6 - ТУ установки
        t->setItem(row,6, new QTableWidgetItem (route->srcTuSet));
        if (route->tuSetError)
        {
            t->item(row,6)->setText(QString("%1  ОК= %2").arg(t->item(row,5)->text()).arg(route->GetTuSetEnum()));
            t->item(row,6)->setForeground(Qt::red);
            t->item(row,6)->setBackground(Qt::yellow);
            errors++;
        }

        // 7 - ТУ отмены
        t->setItem(row,7, new QTableWidgetItem (route->srcTuCancel));
        if (route->tuSetError)
        {
            t->item(row,7)->setText(QString("%1  ОК= %2").arg(t->item(row,5)->text()).arg(route->GetTuCancelEnum()));
            t->item(row,7)->setForeground(Qt::red);
            t->item(row,7)->setBackground(Qt::yellow);
            errors++;
        }

        // 8 - Замыкание
        t->setItem(row,8, new QTableWidgetItem (route->srcZzmk));
        if (route->srcZzmkError)
        {
            t->item(row,8)->setForeground(Qt::red);
            t->item(row,8)->setBackground(Qt::yellow);
            errors++;
        }

        // 9 - Контроль набора маршрута
        t->setItem(row,9, new QTableWidgetItem (route->srcHangSetMode));
        if (route->hangSetModeError)
        {
            t->item(row,9)->setForeground(Qt::red);
            t->item(row,9)->setBackground(Qt::yellow);
            errors++;
        }

        // 10 - Отмена набора
        t->setItem(row,10, new QTableWidgetItem (route->srcTuCancelPending));
        if (route->tuCancelPendingError)
        {
            t->item(row,10)->setForeground(Qt::red);
            t->item(row,10)->setBackground(Qt::yellow);
            errors++;
        }

        // 11 - Контроль направления перегона
        t->setItem(row,11, new QTableWidgetItem (route->srcPrgTsExpr));
        if (route->srcPrgTsError)
        {
            t->item(row,11)->setForeground(Qt::red);
            t->item(row,11)->setBackground(Qt::yellow);
            errors++;
        }

        // 12 - ТУ разворот
        t->setItem(row,12, new QTableWidgetItem (route->srcPrgTu));
        if (route->srcPrgTuError)
        {
            t->item(row,12)->setForeground(Qt::red);
            t->item(row,12)->setBackground(Qt::yellow);

            errors++;
        }

        row++;
    }
    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::AscendingOrder);      // сортировка по умолчанию
    t->resizeColumnsToContents();

    ui->labelErrors->setText(QString::number(errors));      // число ошибок описания
}

void DlgRoutes::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
    {
        // обновить статус маршрутов
        QTableWidget * t = ui->tableRoutes;
        for (int i=0; i<t->rowCount(); i++)
        {
            QTableWidgetItem * item = t->item(i,1);
            Route * route = (Route *) item->data(Qt::UserRole).value<void*>();
            if (route==nullptr)
                continue;
//          item->setIcon(getStsImage(route));
            item->setBackground(getBrush(route));
        }
    }
}

// получить значок состояния ТС
QIcon DlgRoutes::getStsImage (class Route * route)
{
    return route->sts==Route::PASSIVE ? *g_white :
           route->IsOpen()            ? route->IsManevr() ? * g_yellow : *g_green :
                                        *g_cyan;
}

// получить кисть по состоянию ТС
QBrush DlgRoutes::getBrush   (class Route * route)
{
    return route->sts==Route::PASSIVE ? Qt::white :
           route->IsOpen()            ? route->IsManevr() ? Qt::yellow : Qt::green :
                                        Qt::cyan;
}

