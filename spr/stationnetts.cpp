#include "stationnetts.h"
#include "station.h"


StationNetTS::StationNetTS()
{
    memset(this, 0, sizeof(StationNetTS));
    signature = SIGNATURE;
    length = sizeof (StationNetTS);
    realDataLen = 0;
}

int StationNetTS::PutTsToBuffer(Station * st)
{
    nost	= st->No();							// номер станции
    // опирование ТС
    memcpy (inputData, st->lineData, std::min(st->realLinetDataLength, (WORD)sizeof(inputData)));

    //realDataLen = RealInputDataLen;		// длина блока данных из линии

    //mainLineCPU = st->;			// -1/0/1/2 (отказ/откл/WAITING/OK)
    //ssrvLineCPU = dt->;			// -1/0/1/2 (отказ/откл/WAITING/OK)

    rsrv	= st->IsRsrv() ? 1 : 0;
    // seans	= (BYTE)Seans;					// номер сеанса связи
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
