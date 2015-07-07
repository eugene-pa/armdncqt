#include "station.h"
#include "rc.h"
#include "svtf.h"
#include "strl.h"
#include "streamts.h"
#include "dstdatafromfonitor.h"

// 2009.12.11. Дополнительные данные, получаемые из модуля управление
BYTE DOptionsDataFromMonitor::mntrIP3;                      // третий октет IP-адреса актуального АРМ ДНЦ
BYTE DOptionsDataFromMonitor::mntrIP4;                      // четвертый октет IP-адреса актуального АРМ ДНЦ
BYTE DOptionsDataFromMonitor::mntrVer4;                     // четвертый октет номера версии модуля УПРАВЛЕНИЕ
bool DOptionsDataFromMonitor::mntr3pages;                   // признак поддержки 3х страниц (опция LARGE_TS(_KBSH))
bool DOptionsDataFromMonitor::mntrOtuOk;                    // признак НОРМА связи с БПДК/УПОК

bool bTimeMarker = false;                                   // 2015.06.05 вер.2.0.0.303

// Класс DDataFromMonitor и вспомогательные классы DStDataFromMonitor,DOptionsDataFromMonitor,DRcDataFromMonitor,DTrainsDataFromMonitor
// инкапсулируют содержательную частьпротокола обмена по сети между MONITOR и его клиентами (TABLO, АРМ ШНЦ и т.д.)
// Каждый из вспомогат.классов имеет члены-данные, кот.передаются по сети и 2 ф-ии: Prepare() для передающей стороны и Extract() для принимающей
// (Не каждый, оказалось разумным перенести ф-ии в классы поездов и РЦ)
// ==============================================================================================================================================================
// 2012.04.04. Для интеграции нескольких кругов в один круг используется механизм подстановок идентификаторов
// В потоке ТС должны быть индексированы следующие идентификаторы:
// - станции
// - перегоны
// - РЦ
// - маршруты
// По стрелкам и светофорам никакая объектная информация не передается, их состояние вычисляется на основе ТС
// ==============================================================================================================================================================
BYTE DDataFromMonitor::DataBuf[MAX_DATA_LEN_FROM_MONITOR];

// извлечение данных из потока
// сигнатуру можно не проверять, так как весь блок - обрамлен обязательно
void DDataFromMonitor::Extract(UINT length, int bridgeno/* = 0*/)
{
    if (length)
    {
        if (Signature == SIGNATURE)
        {
            if (ExtractOptionsInfo())                       // 1. Общие параметры
            {
                ExtractStInfo(bridgeno);                    // 2. Информация по станциям
//                #ifdef _ARM_TOOLS
//                if (!m_QuickSearching)					// Если АРМ ШН - проверим, не включен ли режим поиска; если включен - не обрабатываем РЦ и поезда
//                #endif
//                {
                //ExtractRcInfo	 (bridgeno);                // 3. информация по РЦ
                //ExtractTrainsInfo(bridgeno);                // 4. информация по поездам
//                }
            }
        }
        else
            Logger::LogStr("Нет сигнатуры в пакете от MONITOR");
    }
    else
        Logger::LogStr ("Нулевая длина при вызове DDataFromMonitor::Extract");    // вывод строки в актуальный лог (если задан) или в отладочное окно
}

bool DDataFromMonitor::ExtractOptionsInfo()
{
    return ((DOptionsDataFromMonitor*)GetPtrOptionsInfo())->Extract(GetLenOptionsInfo());
}

void DDataFromMonitor::ExtractStInfo (int bridgeno /*= 0*/)
{
    if (GetLenStInfo() >= nSt * LenOneSt)
    {
        BYTE * p = GetPtrStInfo();
        for (int i=0; i<nSt; i++)
        {
            // Здесь можно выполнить подстановку и передать в функцию сразу указатель на справочник!
            DStDataFromMonitor * pStData = (DStDataFromMonitor	  *)p;
            Station * pSt = NULL;
//			if (BridgeNo)
//			{
//				pSt = DStation::GetSprByNoOrgAndKrug(pStData->NoSt, BridgeNo);		// 2012.04.04.
//				if (!pSt)
//					continue;														// обрабатываем только идентифицированные данные
//			}

            pStData->Extract(pSt, this, NULL);

            p += LenOneSt;
        }

    }
}


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
                    // нужен универсальный способ изменения времени системы
                    time_t t = tmdt;
                    struct tm * pTm = localtime(&t);
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
