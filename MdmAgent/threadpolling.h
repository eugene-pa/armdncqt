#ifndef THREADPOLLING_H
#define THREADPOLLING_H
#include "../common/common.h"


class Station * NextSt();                                       // получить след.станцию для опроса
class Station * actualSt;                                       // актуальная станция

void ThreadPolling(long param);                                 // рабочий поток опроса каналов
Station * TryOneChannel(class BlockingRS *, class RasPacker*);  // опрос по заданному каналу
bool GetData   (BlockingRS * rs);                               // прием пакета от КП по COM-порту
bool GetDataNet(BYTE * data, int length);                       // обработка пакета КП, принятых по сети

#endif // THREADPOLLING_H
