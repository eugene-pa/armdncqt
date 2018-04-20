#ifndef THREADPOLLING_H
#define THREADPOLLING_H
#include "../common/common.h"


class Station * NextSt();                                       // получить след.станцию для опроса
class Station * actualSt;                                       // актуальная станция

void ThreadPolling(long param);                                 // рабочий поток опроса каналов
Station * TryOneChannel(class BlockingRS *, class RasPacker*);  // опрос по заданному каналу
bool GetData(BlockingRS * rs);                                  // прием данных

#endif // THREADPOLLING_H
