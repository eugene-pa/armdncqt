#include "train.h"

QHash  <int, Train *> Train::Trains;                        // поезда , индексированные по системному номеру
QStack <Train> Train::FreeTrains;                           // пул удаленных справочников для повторного использования


Train::Train()
{

}

Train::~Train()
{

}

// получить справочник по номеру РЦ
Train * Train::GetBySysNo(int sno)
{
    return Trains.contains(sno) ? Trains[sno] : nullptr;
}

//
Train * Train::GetSprByOrgNoAndKrug(int sno, int bridgeno)
{
    Q_UNUSED(bridgeno)
    return GetBySysNo(sno);
}
