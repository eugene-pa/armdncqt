#include "dlgsvtfinfo.h"
#include "ui_dlgsvtfinfo.h"
#include "../spr/station.h"

DlgSvtfInfo::DlgSvtfInfo(Station * st, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSvtfInfo)
{
    this->st = st;
    ui->setupUi(this);

    fillData();
    startTimer(1500);
}

DlgSvtfInfo::~DlgSvtfInfo()
{
    delete ui;
}

void DlgSvtfInfo::fillData()
{
    setWindowTitle("Состояние светофоров по ст." + st->Name());
    ui->treeSvtf->setColumnCount(3);
    ui->treeSvtf->setHeaderLabel("Состояние светофоров по станции " + st->Name());
    ui->treeSvtf->setHeaderLabels(QStringList() << "Светофор" << "Состояние" << "#" );
    ui->treeSvtf->setSortingEnabled(false);

    foreach (Svtf * svtf, st->Allsvtf())
    {

        QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << svtf->name << " " << "#" + QString::number(svtf->Id()));
        ui->treeSvtf->addTopLevelItem(item);
        item->setData(0, Qt::UserRole,qVariantFromValue((void *)svtf));    // запомним стрелку
        item->setIcon(0, svtf->IsOpen() ? * g_green : * g_white);

        if (svtf->formula_er != nullptr)
        {
            QTreeWidgetItem * itemA  = new QTreeWidgetItem(QStringList() << "Авария:" << svtf->formula_er->Source());
            item->addChild(itemA);
            itemA->setIcon(1, svtf->IsAlarm() ? *g_red_box : * g_white_box);      // идентификатор
        }
        QTreeWidgetItem * itemTs  = new QTreeWidgetItem(QStringList() << "ТС");
        item->addChild(itemTs);
        foreach (Ts * ts, svtf->tsList)
        {
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << ts->Name() << ts->Place());
            itemTs->addChild(child);
            child->setIcon(1, ts->Sts() ? *g_green_dark_box : * g_white_box);      // идентификатор
            child->setData(0, Qt::UserRole,qVariantFromValue((void *)ts));    // запомним ТС
        }

        QTreeWidgetItem * itemTu = new QTreeWidgetItem(QStringList() << "ТУ");
        item->addChild(itemTu);
        foreach (Tu * tu, svtf->tuList)
        {
            // показываю колодку, контакт, но для виртуальных ТУ там нули, можно дать всю цепочку ТУ
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << tu->Name() << tu->Place() + " = " + tu->TuEnum());
            itemTu->addChild(child);
        }
    }

    ui->treeSvtf->setAlternatingRowColors(true);
    ui->treeSvtf->setSortingEnabled(true);
    ui->treeSvtf->sortByColumn(0,Qt::AscendingOrder);
}

void DlgSvtfInfo::UpdateStatus()
{

}

void DlgSvtfInfo::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        UpdateStatus();
}

void DlgSvtfInfo::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}

// смена станции
void DlgSvtfInfo::changeStation(class Station * p)
{
    if (p!=st)
    {
        ui->treeSvtf->clear();
        st = p;
        fillData();
        update();
    }
}

void DlgSvtfInfo::on_buttonBox_accepted()
{
    close();
}

void DlgSvtfInfo::on_buttonBox_rejected()
{
    close();
}
