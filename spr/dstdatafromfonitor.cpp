#include "streamts.h"
#include "dstdatafromfonitor.h"
#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"

// сформировать данные
int DStDataFromMonitor::Prepare(class Station * pSt)
{
    // т.к.класс Station имееем friend class DStDataFromMonitor, имеем доступ к закрытым членам
    Signature	= SIGNATURE;                                // обрамление
    NoSt		= pSt->no;                                  // номер станции
    reserv1		= (BYTE)pSt->version;                       // номер версии КП
    TurnOn		= pSt->stsOn    ? 1 : 0;        			// Вкл / Откл
    Active		= pSt->stsActual? 1 : 0;                    // актуальная станция (управляемая) А НЕ опрашивается в данный момент
    StsRu		= pSt->stsRu    ? 1 : 0;                    //
    StsSu		= pSt->stsSu    ? 1 : 0;                    //
    StsMu		= pSt->stsMu    ? 1 : 0;                    //
    ByByLock	= pSt->bybylogic? 1 : 0;                    //
    // признак ЭЦ.ОШБ - 1, если включен контроль и есть ошибка логического контроля
    stsErrorLogic =     Station::LockLogicEnable &&  (pSt->IsActualErrorLockMsgPresent() || pSt->IsNewErrorLockMsgPresent()) ? 1 : 0;
    srsTsExpired = pSt->IsTsExpire() ? 1 : 0;               // устарели ТС
    BackChannel = pSt->IsBackChannel() ? 1 : 0;             // признак опроса с обратного канала для КП-2000
    StsAu       = pSt->stsAu ? 1 : 0;                       //

    memmove (TS		,pSt->tsSts     .data_ptr()->data()+1, qMin((int)sizeof(TS     ),pSt->tsSts     .size()/8+1 ));	// 0/1
    memmove (PulseTS,pSt->tsStsPulse.data_ptr()->data()+1, qMin((int)sizeof(PulseTS),pSt->tsStsPulse.size()/8+1 ));	// мигание

    reserv2_1	= pSt->addr;                                // алрес ЛПУ на линии		вер.2.0.0.186 (пока при приеме не используется)
    reserv2_2	= pSt->ras;                                 // номер станции связи		вер.2.0.0.186 (пока при приеме не используется)
    IgnoreError	= 1;                                        // игнорировать некооректные ТС. УСТАРЕЛО, всегда игнор! pSt->IgnoreError ? 1 : 0;
    IgnoreError |= pSt->gidUralId << 1;                     // объединяем с кодом КП станции
    Rsrv		= pSt->stsRsrv ? 1 : 0;						// резерв 31 бит

/*
    // 2015.03.04. вер.2.0.0.297. Используем биты d0-d4 поля Rsrv_EX1
    // 2015.06.05  вер.2.0.0.303. Опционирую передачу опцией TIMEMARKER, которая читается в bTimeMarker
    if (bTimeMarker)
        Rsrv_EX1 = CTime::GetCurrentTime().GetHour() & 0x1f; // ЗНАЧЕНИЕ АСТРОНОМИЧЕСКОГО ЧАСА ДЛЯ КОРРЕКТНОЙ ОБРАБОТКИ СМЕНЫ ЧАОВЫХ ПОЯСОВ при возможной их смене


    MainLineCPU = pSt->MainLineCPU;					// -1/0/1/2 (отказ/откл/WAITING/OK)
    RsrvLineCPU = pSt->RsrvLineCPU;					// -1/0/1/2 (отказ/откл/WAITING/OK)

    for (int i=0; i<DUBL; i++)
    {
        LinkError[i]=pSt->LinkError[i];				// Тип ошибки: 0-OK,1-молчит,2-CRC
        CntLinkEr[i]=pSt->CntLinkEr[i];				// Общий счетчик ошибок связи
        LastTime [i]=pSt->LastTime [i];				// Астр.время окончания последнего цикла ТС

        SysStatus[i]=pSt->SysStatus[i];
        SpeedCom3[i]=pSt->SpeedCom3[i];
        RcnctCom3[i]=pSt->RcnctCom3[i];
        SpeedCom4[i]=pSt->SpeedCom4[i];
        RcnctCom4[i]=pSt->RcnctCom4[i];

        // В КП-2007 использую эти поля для передачи отказов модулей ТУ/ТС
        if (pSt->IsKp2007())
        {
            SpeedCom3[i] /= 1200;
            SpeedCom4[i] /= 1200;
            memmove (((BYTE*)&SpeedCom3[i])+1,pSt->GetPtrDiag2007MT(i)    ,3);
            memmove (((BYTE*)&SpeedCom4[i])+1,pSt->GetPtrDiag2007MT(i) + 3,3);
//			* (((BYTE *)&tSpokSnd) + 3) = pSt->GetDiag2007MVV(i);
        }

        tSpokSnd = pSt->tSpokSnd;
        tSpokRcv = pSt->tSpokRcv;
        if (pSt->IsKp2007())
        {
//			WORD * pMem = (WORD *)&tSpokRcv;
//			pMem[0] = pSt->GetStatusExWord(0);	//	GetFreeMem(0);
//			pMem[1] = pSt->GetStatusExWord(1);	//	GetFreeMem(1);

            BYTE * p = (BYTE *)&tSpokRcv;
            p[0] = pSt->VersionNo	[0];
            p[1] = pSt->StatusEx	[0];
            p[2] = pSt->VersionNo	[1];
            p[3] = pSt->StatusEx	[1];

        }
    }

    // В КП-2007 использую tSpokSnd для передачи MkuStatus и состояния модулей МВВ
    if (pSt->IsKp2007())
    {
        * (((BYTE *)&tSpokSnd) + 0) = pSt->GetDiag2007MVV(0);
        * (((BYTE *)&tSpokSnd) + 1) = pSt->GetDiag2007MVV(1);
        * (((BYTE *)&tSpokSnd) + 2) = pSt->MkuStatus[0];
        * (((BYTE *)&tSpokSnd) + 3) = pSt->MkuStatus[1];

    }

*/
    return sizeof (DStDataFromMonitor);
}
