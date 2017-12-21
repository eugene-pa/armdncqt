#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mutex"
#include "../common/logger.h"
#include "../common/sqlmessage.h"
#include "../common/sqlserver.h"
#include "../common/sqlblackbox.h"
#include "../spr/stationbase.h"

std::timed_mutex exit_lock;									// мьютекс, разрешающий завершение приложения

QString dbname = "/home/dnc/projects/armdncqt/apo/03.Армавир-Белореченская/bd/arm.db";
QString mainstr = "DRIVER=QPSQL;Host=192.168.0.107;PORT=5432;DATABASE=blackbox;USER=postgres;PWD=358956";
QString rsrvstr = "";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    exit_lock.lock();                                       // блокируем мьютекс завершения
    ui->setupUi(this);

    logger = new Logger("LOG/trstpsql.log", true, true);
    StationBase::ReadBd(dbname, 0, *logger);

    blackbox = new SqlBlackBox (mainstr, rsrvstr, logger);
    //blackbox->putMsg(1, "Тест записи 1", APP_MONITOR , LOG_TU);
    //blackbox->putMsg(3, "Тест записи 3", APP_MDMAGENT, LOG_NOTIFY);

    QTableWidget * t = ui->tableWidget;
    t->setColumnCount(8);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setHorizontalHeaderLabels(QStringList() << "Дата" << "Время" << "Станция" << "Сообщение" << "Хост" << "Приложение" << "Тип" << "Архивировано" );

    // автоматически растягтваем 8-й столбец
    t->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    UpdateList();
}

MainWindow::~MainWindow()
{ 
    exit_lock.unlock();                                     // разблокируем мьютекс завершения
    delete blackbox;
    delete ui;
}

void MainWindow::on_pushButtoWrite_clicked()
{
    //blackbox->putMsg(1, "Тест записи 101", APP_MONITOR , LOG_TU);
    blackbox->putMsg(3, "Тест записи 103", APP_MDMAGENT, LOG_NOTIFY);
    UpdateList();
}

void MainWindow::UpdateList()
{
    QTableWidget * t = ui->tableWidget;

    SqlServer * svr = blackbox->GetSvr(0);
    QSqlDatabase db = QSqlDatabase::database(svr->Name());
    if (db.isOpen())
    {
        QSqlQuery n("SELECT COUNT (id) as cnt FROM public.messages;",db);
        int rows = n.next() ? n.value("cnt").toInt() : 0;
         t->setRowCount(rows);

        QSqlQuery q("SELECT * FROM messages", db);
        int row = 0;
        while (q.next())
        {
            bool ret;
            QDateTime tmdt = q.value("dttm").toDateTime();
            QTableWidgetItem * item;
            t->setItem(row,0, item = new QTableWidgetItem (tmdt.toString(FORMAT_DATE)));
            t->setItem(row,1, new QTableWidgetItem (tmdt.toString(FORMAT_TIME)));

            int idst = q.value("idst").toInt(&ret);
            StationBase * st = StationBase::GetById(q.value("idst").toInt(&ret));
            t->setItem(row,2, new QTableWidgetItem (st==nullptr ? "ПУ" : st->Name()));

            t->setItem(row,3, new QTableWidgetItem (q.value("message").toString()));
            t->setItem(row,4, new QTableWidgetItem (q.value("host"   ).toString()));

            //QBrush brush = getBackground(rc);
            //if (brush != item->background())
            item->setBackground(Qt::yellow);

            row++;
        }
    }

    t->resizeColumnsToContents();
    t->setSortingEnabled(true);                         // разрешаем сортировку
    t->sortByColumn(1, Qt::AscendingOrder);             // сортировка по умолчанию по №
}
