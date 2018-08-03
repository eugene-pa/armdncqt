#ifndef THREADPOLLING_H
#define THREADPOLLING_H
#include "../common/common.h"


extern class Station * actualSt;                                // актуальная станция
extern qint8   statusMain;                                      // прямой   канал: -1/0/1/2 - не готов/нет в конфиге/нет CD/ CD
extern qint8   statusRsrv;                                      // обратный канал: -1/0/1/2 - не готов/нет в конфиге/нет CD/ CD

class Station * NextSt();                                       // получить след.станцию для опроса
void ThreadPolling(long param);                                 // рабочий поток опроса каналов
Station * TryOneChannel(class BlockingRS *, class RasPacker*);  // опрос по заданному каналу
bool GetData   (BlockingRS * rs);                               // прием пакета от КП по COM-порту
bool GetDataNet(BYTE * data, int length);                       // обработка пакета КП, принятых по сети



#endif // THREADPOLLING_H
