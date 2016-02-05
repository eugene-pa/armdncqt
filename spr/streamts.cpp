#include "streamts.h"
#include "krug.h"

// 2009.12.11. Дополнительные данные, получаемые из модуля управление
BYTE DOptionsDataFromMonitor::mntrIP3;                      // третий октет IP-адреса актуального АРМ ДНЦ
BYTE DOptionsDataFromMonitor::mntrIP4;                      // четвертый октет IP-адреса актуального АРМ ДНЦ
BYTE DOptionsDataFromMonitor::mntrVer4;                     // четвертый октет номера версии модуля УПРАВЛЕНИЕ
bool DOptionsDataFromMonitor::mntr3pages;                   // признак поддержки 3х страниц (опция LARGE_TS(_KBSH))
bool DOptionsDataFromMonitor::mntrOtuOk;                    // признак НОРМА связи с БПДК/УПОК

bool bTimeMarker = false;                                   // 2015.06.05 вер.2.0.0.303


// ==============================================================================================================================================================
//								class DOptionsDataFromMonitor
// ==============================================================================================================================================================
//  извлечь данные
bool DOptionsDataFromMonitor::Extract(UINT length)
{
    if (length == sizeof(DOptionsDataFromMonitor))
    {
        if (IsArmTools ())
        {
            // 1. синхронизация времени удаленного клиента по времени сервера: можно попробовать отказаться от этой функции
            // устаревание будем отслеживать по флагу устаревания!
            // 2. отслеживание разницы часовых поясов между серверным временем и локальным (dt > 3000)
            g_TmDtServer = tmdt;                            // Запомним время на сервере
            g_DeltaTZ = 0;                                  // 2014.10.29. Хочу оценить разницу установок времени
            time_t t; time (&t);                            // тек.время
            if (abs(t - tmdt) > 3000)                       // если разница с сервером порядка часа - учтем ее
            {
                g_DeltaTZ = tmdt - t;                       // разница с сервером
                g_TmDtServer -= g_DeltaTZ;
            }

            if (!g_bIgnoreServerTime                        // Если нет флага ИГНОРИРОВАТЬ серверное время
                && !g_bNoSetServerTime)                     // И ФЛАГА НЕ СИНХРОНИЗИРОВАТЬ!
            {                                               //  синхронизируем время на клиенте при расхождении более 10 сек
                if (abs(t - tmdt) >10 )
                {
                    // нужно уметь измененить время разных ОС
                    time_t t = tmdt;
                    struct tm _tm;
                    localtime_s(&_tm, &t);
                    //struct tm * pTm = localtime_s(&t);
//                    SYSTEMTIME st = {1900+pTm->tm_year,pTm->tm_mon+1,pTm->tm_wday,pTm->tm_mday,pTm->tm_hour,pTm->tm_min,pTm->tm_sec,0 };
//                    SetLocalTime(&st);
                }
            }

        }
        else
        if (IsTablo ())
        {
            // 1. параметры отображения
          g_ShowStrlText  = ShowStrlText  ? true:false;     // показать надписи стрелок
          g_ShowSvtfText  = ShowSvtfText  ? true:false;     // показать надписи светофоров
          g_ShowClosedSvtf= ShowClosedSvtf? true:false;     // показать закрытые светофоры

            // 2. время на табло - можно упразднить (там где есть Табло, оно работает на том же ПК)
        }

        mntr3pages	= large ? true : false;
        mntrOtuOk	= OtuOk ? true : false;
        mntrIP3 = ip3;                                      // 3-й октет IP-адреса модуля Управление (2015.03.18 - ЯВНО)
        mntrIP4	= ip4,                                      // 4-й октет IP-адреса модуля Управление
        mntrVer4= version;                                  // 4-й октет номера версии

        return true;
    }
    else
    {
        Logger::LogStr("Некорректная длина блока DOptionsDataFromMonitor");
        return false;
    }

}


// ==============================================================================================================================================================
//								class DPrgDataFromMonitor
// ==============================================================================================================================================================
void DPrgDataFromMonitor::Extract(KrugInfo * krug)
{
    if (Signature != SIGNATURE)
    {
        TRACE("Нет сигнатуры");
        return;
    }

    Peregon *prg = Peregon::GetById (NoPrg, krug);
    if (prg)
    {
//        prg->ChdkOn		 = ChdkOn;              // вкл/окл контроль поездов по ЧДК
//        prg->AllEvnTrains= AllEvnTrains;		// число поездов в четном напр.
//        prg->AllOddTrains= AllOddTrains;		// число поездов в нечетном напр.
//        memmove(pPrg->EvnTrains,EvnTrains,sizeof(EvnTrains));
//        memmove(pPrg->OddTrains,OddTrains,sizeof(OddTrains));
    }

}


