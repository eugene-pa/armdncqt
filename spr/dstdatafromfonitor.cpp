#include "streamts.h"


static DStDataFromMonitor ActualStData;                     // статический экземляр, использующейся для распаковки актуального станционного блока


// сформировать данные
int DStDataFromMonitor::Prepare(Station * pSt)
{
    memset(this, 0, sizeof(DStDataFromMonitor));

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

    PrepareSysInfo (0, pSt->mainSysInfo);                  // основной  - [0]
    PrepareSysInfo (1, pSt->rsrvSysInfo);                  // резервный - [1]


    if (pSt->Kp2007())
    {
        // В КП-2007 использую tSpokSnd для передачи версии и расширенного статцса БМ основного и резервного блоков
        byteof(tSpokRcv,0) = pSt->mainSysInfo->LoVersionNo();// версия основного БМ
        byteof(tSpokRcv,1) = pSt->mainSysInfo->SysStatusEx();// расширенный статус основного БМ
        byteof(tSpokRcv,2) = pSt->rsrvSysInfo->LoVersionNo();// версия резевного БМ
        byteof(tSpokRcv,3) = pSt->rsrvSysInfo->SysStatusEx();// расширенный статус резевного БМ

        // В КП-2007 использую tSpokSnd для передачи MkuStatus и состояния модулей МВВ
        byteof(tSpokSnd,0) = pSt->mainSysInfo->MVVStatus();// статус МВВ основного БМ
        byteof(tSpokSnd,0) = pSt->rsrvSysInfo->MVVStatus();// статус МВВ резевного БМ
        byteof(tSpokSnd,0) = pSt->mainSysInfo->MKUStatus();// статус МКУ основного БМ
        byteof(tSpokSnd,0) = pSt->rsrvSysInfo->MKUStatus();// статус МКУ резевного БМ
    }

    return sizeof (DStDataFromMonitor);
}

// сформировать блок данных системной информации
void DStDataFromMonitor::PrepareSysInfo (int i, SysInfo* info)
{
    LinkError[i] = info->linestatus;                         // Тип ошибки: 0-OK,1-молчит,2-CRC
    CntLinkEr[i] = info->errors;                             // Общий счетчик ошибок связи
    LastTime [i] = info->tmdt.toTime_t();                    // Астр.время окончания последнего цикла ТС
    SysStatus[i] = info->SysStatus();                        // состояние SysStatus

    mvv1[i].speedCom3l = info->SpeedCom3();                  // скорость Com3
    RcnctCom3[i] = info->BreaksCom3();                       // число реконнектов Com3

    mvv2[i].speedCom4l = info->SpeedCom4();                  // скорость Com4
    RcnctCom4[i] = info->BreaksCom4();                       // число реконнектов Com4

    // В КП-2007 использую эти поля для передачи отказов модулей ТУ/ТС
    if (info->st != nullptr)
    {
        if (info->st->Kp2007())
        {
            mvv1[i].speedCom3 = mvv1[i].speedCom3l/1200;    // скорость - в одном байте коэффициентом отеосительно 1200
            mvv2[i].speedCom4 = mvv2[i].speedCom4l/1200;

            mvv1[i].bt1 = info->GetMtuMtsStatus(0);          // отказы модулей
            mvv1[i].bt2 = info->GetMtuMtsStatus(1);
            mvv1[i].bt3 = info->GetMtuMtsStatus(2);
            mvv2[i].bt1 = info->GetMtuMtsStatus(0);
            mvv2[i].bt2 = info->GetMtuMtsStatus(1);
            mvv2[i].bt3 = info->GetMtuMtsStatus(2);
        }

        tSpokSnd = info->st->tSpokSnd;
        tSpokRcv = info->st->tSpokRcv;
    }
}

// извлечь данные. pSt - принудительно указывает станцию
bool DStDataFromMonitor::Extract(Station *st, int realTsLength, DRas *pRas)
{
    // 2012.12.28. Адаптация формата класса с короткими данными (PAGE=1) к формату класса с длинными данными (PAGE=3) за счет сдвига
    bool bDoShortData = realTsLength < TsMaxLengthBytes;
    if (bDoShortData)
    {
        BYTE * p1 = TS,										// начало блока ТС в коротком и длинном пакетах
             * p2 = PulseTS,								// начало блока пульсации в целевом длинном пакете
             * p3 = &reserv2_1;								// указатель на данные за блоками ТС в целевом длинном пакете

        int epilog_length = sizeof (DStDataFromMonitor) - ((BYTE*)&reserv2_1 - (BYTE *) this); // можно и так: (BYTE*)&tSpokRcv - (BYTE*)&reserv2_1 + sizeof(tSpokRcv);
        memmove (p3, p1 + realTsLength * 2, epilog_length);	// сдвигаем данные подвала (остатка за блоком ТС)
        memmove (p2, p1 + realTsLength, realTsLength);      // сдвигаем данные пульсации
    }

    if (Signature != SIGNATURE)
    {
        QString msg("Нет сигнатуры в пакете от MONITOR");
//		LogOutMsg(m_DbDir,s,LOG_NET,LOG_SYS);               // РЕАЛИЗОВАТЬ МЕХАНИЗМ ПРОТОКОЛИРОВАНИЯ И ЛОГОВ В ВИДЕ УНИВЕРСАЛЬНОЙ ФУНКЦИИ
        Logger::LogStr (msg);                               // параметры: сообщение, признак протоколирования, источник, тип, станция
        return false;
    }

    // если класс Station не задан явно, ищем по номеру
    if (st == nullptr)
    {
        if ((st = Station::GetById(NoSt))==nullptr)
            return false;
    }

    TRACE (st->name);

    if (!IsArmTools ())
    {
        //DRailChain::ClearRcInfoForStation  (St.NoSt);     // TODO
        //DRoute    ::ClearRouteStatusForStation (St.NoSt); // TODO
    }

//	Присвоение номера станции из канала некорректно при подстановках в случае многоканального подключения
//  Боюсь, что могу вылезти уши, надо потестировать. Пока обрамляю #ifdef
    if (IsArmTools ())
        st->no          = NoSt;                             // номер станции из потока - сомнительная целесообразность

    st->stsOn       = TurnOn > 0;                           // Вкл / Откл
    st->stsActual   = Active > 0;                           // актуальная станция (управляемая)
    st->stsRu       = StsRu > 0;
    st->stsSu       = StsSu > 0;
    st->stsMu       = StsMu > 0;
    st->bybylogic   = ByByLock > 0;

    st->stsFrmMntrErrorLockMsgPresent = stsErrorLogic>0;    // наличие ЗЦ.ОШБ в базовом удаленном АРМ
    st->stsFrmMntrTsExpired = srsTsExpired;                 // ТС устарели в базовом удаленном АРМ

    // в коде исходного проекта здесь реализовани механизм, позволяющий собирать модуль УПРАВЛЕНИЕ с входным потоком от другого модуля УПРАВЛЕНИЕ
    // с учетом особенностей "смешанных" станций Сетунь; не вдавался в подробности, пока здесь не реализовал; при необходимости - разобраться

    // сохраняем массивы tsStsRaw, tsStsPulse (мне кажется эти массивы не используются)
    memmove (st->tsStsRawPrv  .data_ptr()->data()+1, st->tsStsRaw  .data_ptr()->data()+1, st->tsStsRawPrv  .size()/8+1);
    memmove (st->tsStsPulsePrv.data_ptr()->data()+1, st->tsStsPulse.data_ptr()->data()+1, st->tsStsPulsePrv.size()/8+1);

    //
    memmove (st->tsSts     .data_ptr()->data()+1, TS     , st->tsSts     .size()/8+1);
    memmove (st->tsStsPulse.data_ptr()->data()+1, PulseTS, st->tsStsPulse.size()/8+1);

    st->tsStsRaw = st->tsSts ^ st->tsInverse;               // восстанавливаем исходное состояние по конечному с накладной инверсии

    // IgnoreError - устарело

    // получаем идентификацию в ГИД УРАЛ из удаленного АРМ и сверяем с НСИ
    st->gidUralIdRemote = IgnoreError >> 1;                 // идентификация в ГИД УРАЛ, полученная из удаленного АРМ
    int nEsr = st->gidUralIdRemote / 10;
    if (	nEsr
        &&	st->gidUralIdRemote && st->gidUralId
        &&	st->gidUralIdRemote != st->gidUralId )
    {
        Logger::LogStr(QString("Несоответствие ЕСР-кодов. Ст.%1 %2 != %3 (удал)").arg(st->name, st->gidUralId, st->gidUralIdRemote));
    }

    st->stsRsrv = Rsrv & 0x0001 ? TRUE : FALSE;             // КП на резервном БМ: проверяем младший бит

    SysInfo * info = st->stsRsrv ? st->rsrvSysInfo : st->mainSysInfo; // состояние актуального БМ
    ExtractSysInfo (0, st->mainSysInfo);                    // извлечь SysInfo по основному БМ
    ExtractSysInfo (1, st->rsrvSysInfo);                    // извлечь SysInfo по резервному БМ

    st->tSpokSnd = tSpokSnd;
    st->tSpokRcv = tSpokRcv;

    if (st->Kp2007())
    {

        st->mainSysInfo->LoVersionNo (byteof(tSpokRcv,0));   // версия основного БМ
        st->mainSysInfo->SysStatusEx (byteof(tSpokRcv,1));   // расширенный статус основного БМ
        st->rsrvSysInfo->LoVersionNo (byteof(tSpokRcv,2));   // версия резевного БМ
        st->rsrvSysInfo->SysStatusEx (byteof(tSpokRcv,3));   // расширенный статус резевного БМ

        st->mainSysInfo->MVVStatus(byteof(tSpokSnd,0));      // статус МВВ основного БМ
        st->rsrvSysInfo->MVVStatus(byteof(tSpokSnd,1));      // статус МВВ резевного БМ
        st->mainSysInfo->MKUStatus(byteof(tSpokSnd,2));      // статус МКУ основного БМ
        st->rsrvSysInfo->MKUStatus(byteof(tSpokSnd,3));      // статус МКУ резевного БМ
    }
    else
    {
        // оценка работоспособности ОМУЛа путем сравнения времен передачи/приема
        if (    tSpokSnd                                    // время отправки не нулевое
            &&  tSpokRcv                                    // время приема не нулевое
            &&	tSpokRcv > tSpokSnd                         // время приема больше времени отправки
            &&  qMax(LastTime [0],LastTime [1]) - tSpokSnd < 0 * 5  // а время отправки не более 5 минут
            )
            info->src[0] = info->src[0] | 0x80 ;              // омул в порядке!
    }
    return true;
}

bool DStDataFromMonitor::Extract(Station *st, DDataFromMonitor * pDtFrmMnt, DRas *pRas)
{
    // 2012.12.28. Адаптация формата класса с короткими данными (PAGE=1) к формату класса с длинными данными (PAGE=1) за счет сдвига
    int LenOneSt = pDtFrmMnt->GetLenOneSt();				// размер класса из потока
    int ExtraLength = sizeof(DStDataFromMonitor) - TsMaxLengthBytes * 2;// длина класса без ТС
    g_RealStreamTsLength = (LenOneSt - ExtraLength) / 2;	// размер блока ТС из потока
    if (st)
        st->realStreamTsLength = g_RealStreamTsLength;

    memset (&ActualStData, 0, sizeof(ActualStData));		// обнулить класс на случай коротких данных
    memmove(&ActualStData, this, LenOneSt);					// скопировать данные
    return ActualStData.Extract (st, g_RealStreamTsLength, pRas);
}

void DStDataFromMonitor::ExtractSysInfo (int i, SysInfo* info)
{
    info->linestatus = (LineStatus)LinkError[i];            // Тип ошибки: 0-OK,1-молчит,2-CRC
    info->errors = CntLinkEr[i];                            // Общий счетчик ошибок связи
    info->tmdt = QDateTime::fromTime_t(LastTime [i]);       // Астр.время окончания последнего цикла ТС
    info->SysStatus(SysStatus[i]);                          // состояние SysStatus
    long s = mvv1[i].speedCom3l;
    info->SpeedCom3(info->st->Kp2007() ? s : s/1200);       // скорость Com3 с учетом различия версий
    info->BreaksCom3(RcnctCom3[i]);                         // число реконнектов Com3
    s = mvv2[i].speedCom4l;
    info->SpeedCom4(info->st->Kp2007() ? s : s/1200);       // скорость Com4 с учетом различия версий
    info->BreaksCom4(RcnctCom4[i]);                         // число реконнектов Com4


    // В КП-2007 использую эти поля для передачи отказов модулей ТУ/ТС
    if (info->st != nullptr)
    {
        if (info->st->Kp2007())
        {
            info->SetMtuMtsStatus(0, mvv1[i].bt1);
            info->SetMtuMtsStatus(1, mvv1[i].bt2);
            info->SetMtuMtsStatus(2, mvv1[i].bt3);
            info->SetMtuMtsStatus(3, mvv2[i].bt1);
            info->SetMtuMtsStatus(4, mvv2[i].bt2);
            info->SetMtuMtsStatus(5, mvv2[i].bt3);
        }
    }
}
