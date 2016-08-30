#include "train.h"
#include "krug.h"
#include "rc.h"

std::unordered_map <int, Train *> Train::Trains;            // поезда , индексированные по системному номеру
std::stack <Train*> Train::FreeTrains;                          // пул удаленных справочников для повторного использования


Train::Train()
{
    nrc = 0;
}

Train::Train(int sno, int no, class KrugInfo * krug)
{
    nrc = 0;
    update(sno, no, krug);
    Train::Trains[key(sno)] = this;
}

Train * Train::restore(int sno, int no, class KrugInfo * krug)
{
    qDebug() << "Добавляем поезд в Trains:  Sno=" << sno << "  No=" << no;
    Train * train;
    if (FreeTrains.size())
    {
        train = FreeTrains.top();
        FreeTrains.pop();
    }
    else
        train = new Train(sno, no, krug);
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
    ClearRc();
}

Train::~Train()
{

}

// получить справочник по номеру РЦ
Train * Train::GetBySysNo(int sno, KrugInfo * krug)
{
    Q_UNUSED(krug)
    return Trains.count(sno) ? Trains[sno] : nullptr;
}

// добавить поезд
Train * Train::AddTrain(int sno, int no, KrugInfo * krug)
{
    int key = krug==nullptr ? sno : krug->no() << 16 | sno;    // формируем ключ
    return Trains.count(key) ? Trains[key] : restore(sno, no, krug);
}

// добавить занятую РЦ
// механизм повторного использования места в массие QVector Rc не срабатывает (по крайне мере в эмуляторе LINUX, имеем первый элемент = 0, потом значащий)
// причем nrc не соответствкет размерности Rc
void Train::AddRc(class Rc* rc)
{
    if (nrc < Rc.size())
        Rc[nrc] = rc;
    else
        Rc.push_back(rc);
    nrc++;
}

// очистить список РЦ
void Train::ClearRc()
{
    nrc = 0;
    for (class Rc * rc : Rc)
        rc = nullptr;
}

void Train::ClearAllRc()
{
    for (auto rec : Trains)
    {
        Train * train = rec.second;
        train->ClearRc();
    }
}
