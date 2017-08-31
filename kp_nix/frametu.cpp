#include "frametu.h"
#include "ui_frametu.h"
#include "threads/threadtu.h"
#include "common/acksenum.h"
#include "common/pamessage.h"


FrameTU::FrameTU(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameTU)
{
    ui->setupUi(this);

    QTableWidget * t = ui->table_TODO;                         // заполнение таблицы описания ТY

    t->setColumnCount(3);
    t->setHorizontalHeaderLabels(QStringList() << "Мод" << "Вых" << "t,сек");
    t->setSortingEnabled(false);                                // запрещаем сортировку
//    t->resizeColumnsToContents();
//    t-setDefaultSectionSize ( verticalHeader()->minimumSectionSize () );
}

FrameTU::~FrameTU()
{
    delete ui;
}

void FrameTU::UpdateQueues(class PaMessage)
{

}

// обновить очередь TODO
void FrameTU::UpdateQueueTodo(DWORD tu)
{
    QTableWidget * t = ui->table_TODO;                         // заполнение таблицы описания ТY
    t->insertRow(0);
    t->setItem(0,0, new QTableWidgetItem(QString::number(tu)));
}

void FrameTU::UpdateQueueDone(DWORD tu)
{

}

void FrameTU::UpdateQueueSysy(DWORD tu)
{

}

