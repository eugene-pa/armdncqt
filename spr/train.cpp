#include "train.h"
#include "krug.h"
#include "rc.h"

QHash  <int, Train *> Train::Trains;                        // поезда , индексированные по системному номеру
QStack <Train*> Train::FreeTrains;                          // пул удаленных справочников для повторного использования


Train::Train()
{

}

Train::Train(int sno, int no, class KrugInfo * krug)
{
    update(sno, no, krug);
    Train::Trains[key(sno)] = this;
}

Train * Train::restore(int sno, int no, class KrugInfo * krug)
{
    qDebug() << "Добавляем поезд в Trains:  Sno=" << sno << "  No=" << no;
    Train * train = FreeTrains.length() ? FreeTrains.pop() : new Train(sno, no, krug);
    train->update(sno, no, krug);
    return train;
}

void Train::update(int sno, int no, class KrugInfo * krug)
{
    this->sno = sno;
    this->no  = no;
    this->krug = krug;
    krugno = krug==nullptr ? 0 : krug->no();
    tmdt = QDateTime::currentDateTime();
}

Train::~Train()
{

}

// получить справочник по номеру РЦ
Train * Train::GetBySysNo(int sno, KrugInfo * krug)
{
    Q_UNUSED(krug)
    return Trains.contains(sno) ? Trains[sno] : nullptr;
}

// добавить поезд
Train * Train::AddTrain(int sno, int no, KrugInfo * krug)
{
    int key = krug==nullptr ? sno : krug->no() << 16 | sno;    // формируем ключ
    return Trains.contains(key) ? Trains[key] : restore(sno, no, krug);
}

// добавить занятую РЦ
void Train::AddRc(class Rc* rc)
{
    if (nrc < Rc.length()-1)
        Rc[nrc++] = rc;
    else
        Rc.append(rc);
}

// очистить список РЦ
void Train::ClearRc()
{
    nrc = 0;
    for (int i=0; i<Rc.length(); i++)
        Rc[i] = nullptr;
}

void Train::ClearAllRc()
{
    foreach (Train * train, Trains)
        train->ClearRc();
}
