#include <QStandardItemModel>

#include "dlgstrlinfo.h"
#include "ui_dlgstrlinfo.h"
#include "../spr/station.h"

const QString zmkStrl("И.ЗМК");
const QString muStrl ("МУ");

DlgStrlInfo::DlgStrlInfo(Station * st, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStrlInfo)
{
    this->st = st;
    ui->setupUi(this);

    fillData();
    startTimer(1500);
}

DlgStrlInfo::~DlgStrlInfo()
{
    delete ui;
}

// смена станции
void DlgStrlInfo::changeStation(class Station * p)
{
    if (p!=st && p!=nullptr)
    {
        ui->treeStrl->clear();
        st = p;
        fillData();
        update();
    }
}

void DlgStrlInfo::fillData()
{
    if (st==nullptr)
        return;
    setWindowTitle("Состояние стрелок по ст." + st->Name());
    ui->treeStrl->setColumnCount(3);
    ui->treeStrl->setHeaderLabel("Состояние стрелок по станции " + st->Name());
    ui->treeStrl->setHeaderLabels(QStringList() << "Стрелка" << "Состояние" << "#" );
    ui->treeStrl->setSortingEnabled(false);

    for(auto rec : st->Allstrl())
    {
        Strl * strl = rec.second;
        QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << strl->basename << "" << "#" + QString::number(strl->Id()));
        ui->treeStrl->addTopLevelItem(item);
        item->setData(0, Qt::UserRole,qVariantFromValue((void *)strl));    // запомним стрелку
        item->setIcon(0, strl->isPlus() ? * g_strl_plus : strl->isMinus() ? * g_strl_minus : * g_red_box);

        // создаем листья для всех ТС и обобщенных сигналом замыкания и МУ
        QTreeWidgetItem * itemTs  = new QTreeWidgetItem(QStringList() << "ТС");
        item->addChild(itemTs);
        foreach (Ts * ts, strl->tsList)
        {
            // если есть формула, даем формулу, иначе - координаты
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << ts->Name() << (ts->formula.length() ? ts->formula : ts->Place()));
            itemTs->addChild(child);
            child->setIcon(1, ts->Sts() ? *g_green_box : * g_white_box);      // идентификатор
            child->setData(0, Qt::UserRole,qVariantFromValue((void *)ts));    // запомним ТС
        }
        if (strl->formula_zs)
        {
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << zmkStrl << strl->formula_zs->Source());
            child->setIcon(1, strl->formula_zs->GetValue() ? *g_green_box : * g_white_box);
            itemTs->addChild(child);
        }
        // МУ
        if (strl->formula_mu)
        {
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << muStrl << strl->formula_mu->Source());
            child->setIcon(1, strl->formula_mu->GetValue() ? *g_green_box : * g_white_box);
            itemTs->addChild(child);
        }

        QTreeWidgetItem * itemTu = new QTreeWidgetItem(QStringList() << "ТУ");
        item->addChild(itemTu);
        foreach (Tu * tu, strl->tuList)
        {
            // показываю колодку, контакт, но для виртуальных ТУ там нули, можно дать всю цепочку ТУ
            QTreeWidgetItem * child = new QTreeWidgetItem(QStringList() << "" << tu->Name() << tu->Place() + " = " + tu->TuEnum());
            itemTu->addChild(child);
        }
    }

    ui->treeStrl->setAlternatingRowColors(true);
    ui->treeStrl->setSortingEnabled(true);
    ui->treeStrl->sortByColumn(0,Qt::AscendingOrder);
}

void DlgStrlInfo::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        UpdateStatus();
}

void DlgStrlInfo::closeEvent(QCloseEvent *e)
{
    setVisible(false);
    e->ignore();
}

void DlgStrlInfo::UpdateStatus()
{
    for(int i=0; i < ui->treeStrl->topLevelItemCount(); i++)
    {
        QTreeWidgetItem * item = ui->treeStrl->topLevelItem(i);
        Strl * strl = (Strl *) item->data(0, Qt::UserRole).value<void*>();
        item->setIcon(0, strl->isPlus() ? * g_strl_plus : strl->isMinus() ? * g_strl_minus : * g_red_box);
        if (item->childCount())
        {
            for(int j=0; j < item->childCount(); j++)
            {
                if (item->child(j)->text(0) == "ТС")
                {
                    QTreeWidgetItem * itemTs = item->child(j);
                    for (int child = 0; child < itemTs->childCount(); child++)
                    {
                        Ts * ts = (Ts *) itemTs->child(child)->data(0, Qt::UserRole).value<void*>();
                        if (ts!=nullptr)
                            itemTs->child(child)->setIcon(1, ts->Sts() ? *g_green_box : * g_white_box);
                        else
                        if (strl->formula_zs && (itemTs->child(child)->text(1).indexOf(zmkStrl)>=0))
                            itemTs->child(child)->setIcon(1, strl->formula_zs->GetValue() ? *g_green_box : * g_white_box);
                        else
                        if (strl->formula_mu && (itemTs->child(child)->text(1).indexOf(muStrl)>=0))
                            itemTs->child(child)->setIcon(1, strl->formula_mu->GetValue() ? *g_green_box : * g_white_box);

                    }
                 }
             }
        }
    }
}
