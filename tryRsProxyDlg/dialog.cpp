#include <QThread>
#include "dialog.h"
#include "ui_dialog.h"
#include "../common/pasender.h"
#include "../common/qrsasinc.h"


void ThreadPolling(long param);
std::timed_mutex exit_lock;                                     // блокировка до выхода
PaSender paSender;                                              // один класс для отправки сообщений

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    exit_lock.lock ();											// блокируем мьютекс завершения (ждем освобождения во всех потоках)
    ui->setupUi(this);
    //rs = new RsAsinc("COM3,9600,N,8,1");
    //rs->SetTimeWaiting(10);
    //startTimer(50);
    //pThreadPolling	= new std::thread ( ThreadPolling	, 0);	// поток опроса линни связи

    QThread  * threadRS = new QThread();
    QRsAsinc * qrs = new QRsAsinc("COM3,9600,N,8,1");
    qrs->moveToThread(threadRS);

    connect(this, SIGNAL(startrs()), qrs, SLOT(process()));
    connect(qrs, SIGNAL(finished()), this, SLOT(finished()));
    threadRS->start();
    emit(startrs());
}

Dialog::~Dialog()
{
    exit_lock.unlock();											// блокируем мьютекс завершения (ждем освобождения во всех потоках)
    delete ui;
}

void Dialog::on_pushButton_clicked()
{

}

// обработка событий таймера
void Dialog::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
//    try
//    {
//        while (true)
//        {
//            instr += (char)rs->GetChEx();
//        }
//    }
//    catch (RsException e)
//    {
//        // нет данных; игнорируем
//    }
//    ui->textEdit->setText(instr);

}

void Dialog::on_pushButton_2_clicked()
{
//    QByteArray data = ui->lineEdit->text().toUtf8();
//    rs->Send(data);
}

void Dialog::finished()
{

}
