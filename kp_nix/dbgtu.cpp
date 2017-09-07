#include "dbgtu.h"
#include "ui_dbgtu.h"

#include "common/common.h"
#include "common/acksenum.h"
#include "common/pamessage.h"
#include "common/tu.h"


DbgTu::DbgTu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbgTu)
{
    ui->setupUi(this);
}

DbgTu::~DbgTu()
{
    delete ui;
}

void DbgTu::on_pushButton_MTU_clicked()
{
    Tu::PushTu(1);
}

void DbgTu::on_pushButton_RPC_clicked()
{
    Tu::PushTu(2, SYS_RPC);
}

void DbgTu::on_pushButton_Ebl_clicked()
{
    Tu::PushTu(3, SYS_EBL);
}
