#include "dlgperegoninfo.h"
#include "ui_dlgperegoninfo.h"

DlgPeregonInfo::DlgPeregonInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPeregonInfo)
{
    ui->setupUi(this);
}

DlgPeregonInfo::~DlgPeregonInfo()
{
    delete ui;
}
