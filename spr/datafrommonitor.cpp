#include "streamts.h"

// Класс DDataFromMonitor вместе со вспомогательными классами DStDataFromMonitor,DOptionsDataFromMonitor,DRcDataFromMonitor,DTrainsDataFromMonitor
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
                if (!(IsArmTools() && g_QuickSearching))    // Если не включен режим ускорееного просмотра АРМ ШН - обрабатываем РЦ и поезда
                {
                    ExtractRcInfo	 (bridgeno);            // 3. информация по РЦ
                    ExtractTrainsInfo(bridgeno);            // 4. информация по поездам
                }
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


// обработка информации о станциях и перегонах
// информация о перегонах лежит после информации о станциях
// если после обработки информации о станциях есть "остаточный" блок, интерпретируем его как информацию о перегонах
void DDataFromMonitor::ExtractStInfo (int bridgeno /*= 0*/)
{
    int stationInfoLength = nSt * LenOneSt;                 // 1. длина блока информации по станциям в блоке StInfo

    BYTE * dataptr;
    // 1. обработка информации по станциям
    if (GetLenStInfo() >= stationInfoLength)
    {
        dataptr = GetPtrStInfo();
        for (int i=0; i<nSt; i++)
        {
            // Здесь можно выполнить подстановку и передать в функцию сразу указатель на справочник!
            DStDataFromMonitor * pStData = (DStDataFromMonitor	  *)dataptr;

            Station * pSt = NULL;
            // при обработке одного потока станция ищется однозначно по номеру в потоке, поэтому в функцию обработки передается pSt = NULL
            // при обработке нескольких потоков ТС в одном приложении нет однозначного соответствия номер-станция,
            // поэтому используется переменная bridgeno, задающая № обрабатываемого потока для поиска станции
            // если задан поток, ищем станцию и передаем ее явно в функцию обработки
            if (bridgeno)
            {
                pSt = Station::GetSprByNoOrgAndKrug(pStData->NoSt, bridgeno);
                if (!pSt)
                {
                    qDebug() << "Не идентифицирована станция по номеру потока и номеру станции";
                    continue;								// обрабатываем только идентифицированные данные
                }
            }

            pStData->Extract(pSt, this, NULL);
            dataptr += LenOneSt;
        }
    }


    // 2. обрабатываем информацию по перегонам
    int peregonInfoLength = GetLenStInfo() - stationInfoLength;  // 2. длина блока информации по перегонам в блоке StInfo
    if (peregonInfoLength)
    {
        if (peregonInfoLength % sizeof(DPrgDataFromMonitor))
            Logger::LogStr("Некорректная длина блока PrgInfo",LOG_NET,LOG_SYS);
        else
        {
            for (WORD i=0; i<peregonInfoLength/sizeof(DPrgDataFromMonitor); i++)
            {
                ((DPrgDataFromMonitor *)dataptr)->Extract(bridgeno);
                dataptr += sizeof (DPrgDataFromMonitor);
            }
        }
    }

}


// TODO:
// обработка информация по РЦ
void DDataFromMonitor::ExtractRcInfo(int bridgeno)
{
    DRcDataFromMonitor * rcinfo = (DRcDataFromMonitor *)GetPtrRcInfo();
    int length = GetLenRcInfo();
    COneRc *datarc = (COneRc *)(GetPtrRcInfo() + sizeof (DRcDataFromMonitor));

    if (length == sizeof(DRcDataFromMonitor) + rcinfo->m_nRc * sizeof(COneRc))
    {
        for (int i=0; i<rcinfo->m_nRc; i++, datarc++)
        {

            // Вот тут должна делаться подстановка номера РЦ. Вопрос: как определить от какой РЦ. Надо уметь искать РЦ по старому номеру РЦ и номеру круга станции
            Rc * rc = Rc::GetByNo(datarc->NoRc);
            if (bridgeno > 0)
            {
                rc = Rc::GetSprByOrgNoAndKrug (datarc->NoRc, bridgeno);
                if (rc==NULL)
                    continue;
            }
/*
            pRc->SetSNoTr(p->SNo);
            pRc->stsMRSHR_RQ	= p->stsMRSHR_RQ;
            pRc->stsPASS		= p->stsPASS;
            pRc->stsMRSHR_OK	= p->stsMRSHR_OK;
            pRc->FalseZ		= p->stsFalseZ;

            pRc->Rout = BridgeNo == 0 && DRoute::IsValidNo(p->Rout) ? p->Rout : 0;	// 2012.

            // 2009.08.03. Хочу обратным ходом установить состояние маршрута
#ifdef _ARM_TOOLS
            extern bool IgnoreRouresInfo;				// 2012.04,05. Можно игнорировать информацию о маршрутах для отображения в АРМ ШН, Табло
            if (!IgnoreRouresInfo && pRc->Rout)
            {
                // здесь должна быть подстановка маршрутов
                DRoute * pr = NULL;
                if (BridgeNo == 0)
                     pr = &DRoute::GetSpr(pRc->Rout);
                else
                {
                    pRc->Rout = 0;
                    int orgcod = 0;
                    if (DStation::GetSpr(pRc->GetNoSt()).OrgNoRoutToActual.Lookup(pRc->Rout, orgcod )  && DRoute::IsValidNo(orgcod))
                        pr = &DRoute::GetSpr(pRc->Rout=orgcod);
                }
                if (pr)
                {
                    if (p->stsMRSHR_RQ)
                        pr->SetSts (DRoute::RQSET);
                    else
                    if (p->stsMRSHR_OK && !p->stsPASS && pr->GetSts()!=DRoute::WAIT_RZMK)
                        pr->SetSts (DRoute::RQOPEN);
                    else
//					if (p->stsPASS)
                        pr->SetSts (DRoute::WAIT_RZMK);
                }
            }
            else
                pRc->Rout = 0;
#endif // #ifdef ARMTOOLS
*/
        }
    }
    else
        TRACE ("Некорректный формат блока РЦ в потоке");
}

// TODO:
// обработка информация по поездам
void DDataFromMonitor::ExtractTrainsInfo(int bridgeno)
{
    //DListTrains::AllTrains.Extract((DTrainsDataFromMonitor *)GetPtrTrainsInfo(),GetLenTrainsInfo(), BridgeNo);
}

