#include "frametu.h"
#include "ui_frametu.h"
#include "threads/threadtu.h"
#include "common/acksenum.h"
#include "common/pamessage.h"
#include "common/acksenum.h"

FrameTU::FrameTU(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTU)
{
    ui->setupUi(this);

    QTableWidget * t = ui->table_TU;                            // заполнение таблицы описания ТY

    // t->setColumnCount(4);
    t->setHorizontalHeaderLabels(QStringList() << "Мод" << "Вых" << "Тип" << "t,сек");
    t->setSortingEnabled(false);                                // запрещаем сортировку
//    t->resizeColumnsToContents();
//    t-setDefaultSectionSize ( verticalHeader()->minimumSectionSize () );
}

FrameTU::~FrameTU()
{
    delete ui;
}

// обработка сообщений по процессу приема/выдачи ТУ
void FrameTU::UpdateQueues(class PaMessage * pMsg)
{
    QTableWidget * t = ui->table_TU;                         // заполнение таблицы описания ТY
    std::shared_ptr<Tu> tu = pMsg->GetTu();
    switch (pMsg->GetAck())
    {
        case tuAckRcv:
            t->insertRow(0);
            t->setItem(0,0, new QTableWidgetItem(QString::number(tu->GetMod  ())));
            t->setItem(0,1, new QTableWidgetItem(QString::number(tu->GetOut  ())));
            t->setItem(0,2, new QTableWidgetItem(QString::fromStdWString(tu->GetSysName())));
            t->setItem(0,3, new QTableWidgetItem(QString::number(tu->GetDelay())));
            t->item(0,0)->setData(Qt::UserRole,qVariantFromValue(tu->GetId()));    // запомним id
            SelectRow(0,Qt::darkGreen);
            break;
        case tuAckToDo:
            UpdateQueueBeg (tu);
            break;
        case tuAckDone:
            UpdateQueueDone(tu);
            break;

        default:
            break;
    }
}

// обновить очередь TODO
void FrameTU::UpdateQueueBeg (std::shared_ptr<class Tu> tu)
{
    int row = FindById(tu->GetId());
    if (row >= 0)
    {
        ui->table_TU->selectRow(row);
    }
}

void FrameTU::UpdateQueueDone(std::shared_ptr<class Tu> tu)
{
    ui->table_TU->clearSelection();
    int row = FindById(tu->GetId());
    if (row >= 0)
        SelectRow(row,Qt::gray);
}

int FrameTU::FindById(DWORD _id)
{
    QTableWidget * t = ui->table_TU;
    for (int i=0; i<t->rowCount(); i++)
    {
        QTableWidgetItem * item = t->item(i,0);
        DWORD id = item->data(Qt::UserRole).value<DWORD>();
        if (id==_id)
            return i;
    }
    return -1;
}

void FrameTU::SelectRow(int row, QColor clr)
{
    for (int i=0; i<ui->table_TU->columnCount(); i++)
        ui->table_TU->item(row,i)->setForeground(clr);
}
