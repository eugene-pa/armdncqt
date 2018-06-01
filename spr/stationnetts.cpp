#include "stationnetts.h"
#include "station.h"
#include "raspacker.h"


StationNetTS::StationNetTS()
{
    memset(this, 0, sizeof(StationNetTS));
    signature = SIGNATURE;
    length = sizeof (StationNetTS);
    realDataLen = 0;
}

StationNetTS::StationNetTS(Station * st, RasPacker* pack)        // Конструктор
{
    Pack(st, pack);
}

int StationNetTS::Pack (Station * st, RasPacker* pack)
{
    nost	= st->No();							// номер станции
    // копирование длин и блоков C,Т,О,Д
    memcpy (inputData, st->rasData, realDataLen = std::min(st->rasData->Length(), (int)sizeof(inputData)));

    // упаковка состояния прямого и обводного каналов
    //mainLineCPU = st->;                       // -1/0/1/2 (отказ/откл/WAITING/OK)
    //ssrvLineCPU = dt->;                       // -1/0/1/2 (отказ/откл/WAITING/OK)

    rsrv	= st->IsRsrv() ? 1 : 0;
    seans	= 0;                                // номер сеанса связи - не задействован
    backChannel = st->IsBackChannel() ? 1 : 0;	//

    for (int i=0; i<DUBL; i++)
    {
/*
//		NetBuf.BypassSts[i]=BypassSts[i];		// состояние байпаса:

        lastTime [i] = st->LastTime [i];		// Астр.время окончания последнего цикла ТС
        oldTime	 [i] = OldTime  [i];		// Астр.время окончания i-1 цикла ТС
        linkError[i] = LinkError[i];		// Тип ошибки: 0-OK,1-молчит,2-CRC
        cntLinkEr[i] = CntLinkEr[i];		// Общий счетчик ошибок связи
        linkTime [i] = LinkTime [i];		// длительность сеанса
*/
    }
    signature = SIGNATURE;
    SetLenByDataLen(realDataLen);
    return length;
}
