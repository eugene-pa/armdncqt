#include "train.h"
#include "krug.h"

QHash  <int, Train *> Train::Trains;                        // поезда , индексированные по системному номеру
QStack <Train> Train::FreeTrains;                           // пул удаленных справочников для повторного использования


Train::Train()
{

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

