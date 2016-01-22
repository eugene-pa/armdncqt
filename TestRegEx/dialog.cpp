#include "QRegularExpression"
#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    QString regex = ui->lineRegex->text();
    QString src   = ui->lineSource->text();
    QRegularExpressionMatch match;
    ui->lineResult->clear();

    QString result;
    int pos = 0;
    while ((match = QRegularExpression(regex).match(src, pos)).hasMatch())
    {
        result += match.captured() + "  ";
        pos = match.capturedEnd();
    }
    ui->lineResult->setText(result);
}

void Dialog::on_buttonBox_rejected()
{
    close();
}
