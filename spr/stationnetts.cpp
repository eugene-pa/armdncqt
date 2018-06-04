#include "stationnetts.h"
#include "station.h"
#include "raspacker.h"

#pragma pack(1)

StationNetTS::StationNetTS()
{
    memset(this, 0, sizeof(StationNetTS));
    signature = SIGNATURE;
    length = sizeof (StationNetTS);
    realDataLen = 0;
}

StationNetTS::StationNetTS(Station * st)                        // Конструктор
{
    Pack(st);
}

int StationNetTS::Pack (Station * st)
{
    nost	= st->No();                                         // номер станции
    // копирование длин и блоков C,Т,О,Д
    int l = st->rasData->Length();
    memcpy (inputData, st->rasData, realDataLen = std::min(st->rasData->Length(), (int)sizeof(inputData)));

    // упаковка состояния прямого и обводного каналов
    mainLineCPU = st->MainLineCPU;                              // -1/0/1/2 (отказ/откл/WAITING/OK)
    rsrvLineCPU = st->RsrvLineCPU;                              // -1/0/1/2 (отказ/откл/WAITING/OK)

    rsrv	= st->IsRsrv() ? 1 : 0;
    seans	= 0;                                                // номер сеанса связи - не задействован
    backChannel = st->IsBackChannel() ? 1 : 0;                  //


    for (int i=0; i<DUBL; i++)
    {
        SysInfo * info = st->GetSysInfo(i > 0);
        bypassSts[i] = FALSE;                                   // состояние байпаса:

        lastTime [i] = (quint32)info->tmdt    .toTime_t();      // Астр.время окончания последнего цикла ТС
        oldTime	 [i] = (quint32)info->tmdtPrev.toTime_t();      // Астр.время окончания i-1 цикла ТС
        linkError[i] = info->linestatus;                        // Тип ошибки: 0-OK,1-молчит,2-CRC
        cntLinkEr[i] = info->errors;                            // Общий счетчик ошибок связи
        linkTime [i] = lastTime [i] - oldTime[i];               // длительность сеанса

    }
    signature = SIGNATURE;
    SetLenByDataLen(realDataLen);
    return length;
}

void StationNetTS::SetLenByDataLen(WORD datalength)
{
//    length = ((BYTE *)&inputData - (BYTE *)this) + datalength;
    length = sizeof(StationNetTS) - sizeof(inputData) + datalength;
}

#pragma pack()
