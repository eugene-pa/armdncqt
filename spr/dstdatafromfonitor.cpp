#include "streamts.h"
#include "dstdatafromfonitor.h"
#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"

// сформировать данные
int DStDataFromMonitor::Prepare(Station * pSt)
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
    Rsrv_EX1    = QDate::currentDate().day() & 0x1f;        // ЗНАЧЕНИЕ АСТРОНОМИЧЕСКОГО ЧАСА ДЛЯ КОРРЕКТНОЙ ОБРАБОТКИ СМЕНЫ ЧАОВЫХ ПОЯСОВ при возможной их смене
                                                            // Используем биты d0-d4 поля Rsrv_EX1

    // строго говоря, следующие статические данные для круга нет смысла передавать для каждой станции, их нужно было бы передать в блоке общей информации
    MainLineCPU = pSt->MainLineCPU;                         // -1/0/1/2 (отказ/откл/WAITING/OK)
    RsrvLineCPU = pSt->RsrvLineCPU;                         // -1/0/1/2 (отказ/откл/WAITING/OK)

    PrepareSysInfo (0, pSt->mainSysInfo);
    PrepareSysInfo (0, pSt->rsrvSysInfo);

/*
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

void DStDataFromMonitor::PrepareSysInfo (int i, SysInfo& info)
{
    LinkError[i] = info.linestatus;                         // Тип ошибки: 0-OK,1-молчит,2-CRC
    CntLinkEr[i] = info.errors;                             // Общий счетчик ошибок связи
    LastTime [i] = info.tmdt.toTime_t();                    // Астр.время окончания последнего цикла ТС

    SysStatus[i] = info.SysStatus();                        // Байт 0 - состояние SysStatus
    SpeedCom3[i] = info.SpeedCom3();                        // Байт 1 - скорость Com3
    RcnctCom3[i] = info.BreaksCom3();                       // Байт 2 - число реконнектов Com3
    SpeedCom4[i] = info.SpeedCom4();                        // Байт 3 - скорость Com4
    RcnctCom4[i] = info.BreaksCom4();                       // Байт 4 - число реконнектов Com4

    // В КП-2007 использую эти поля для передачи отказов модулей ТУ/ТС
    if (info.st != nullptr)
    {
        if (info.st->Kp2007())
        {
            SpeedCom3[i] /= 1200;
            SpeedCom4[i] /= 1200;
/*
            memmove (((BYTE*)&SpeedCom3[i])+1,pSt->GetPtrDiag2007MT(i)    ,3);
            memmove (((BYTE*)&SpeedCom4[i])+1,pSt->GetPtrDiag2007MT(i) + 3,3);
*/
//			* (((BYTE *)&tSpokSnd) + 3) = pSt->GetDiag2007MVV(i);
        }
/*
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
*/
    }
}
