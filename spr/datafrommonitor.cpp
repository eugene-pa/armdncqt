#include "streamts.h"
#include "krug.h"
#include "train.h"

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
void DDataFromMonitor::Extract(UINT length, KrugInfo * krug)
{
    if (length)
    {
        if (Signature == SIGNATURE)
        {
            if (ExtractOptionsInfo())                       // 1. Общие параметры
            {
                ExtractStInfo(krug);                        // 2. Информация по станциям
                if (!(IsArmTools() && g_QuickSearching))    // Если не включен режим ускорееного просмотра АРМ ШН - обрабатываем РЦ и поезда
                {
                    Train::ClearAllRc();                    // 3. очистить списки РЦ
                    ExtractRcInfo	 (krug);                // 4. информация по РЦ
                    ExtractTrainsInfo(krug);                // 5. информация по поездам
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
void DDataFromMonitor::ExtractStInfo (KrugInfo * krug)
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
            // поэтому используется переменная krug
            if (krug != nullptr)
            {
                pSt = Station::GetById(pStData->NoSt, krug);
                if (!pSt)
                {
                    qDebug() << "Не идентифицирована станция по номеру потока и номеру станции";
                    continue;								// обрабатываем только идентифицированные данные
                }
            }

            pSt = pStData->Extract(pSt, this, NULL);        // извлекакем данные

            // если находимя в режиме поиска событий в архиве - можно не обрабатывать данные
            if (pSt)
                pSt->AcceptTS();                            // обрабатываем станцию

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
                ((DPrgDataFromMonitor *)dataptr)->Extract(krug);
                dataptr += sizeof (DPrgDataFromMonitor);
            }
        }
    }


}


// TODO:
// обработка информация по РЦ
void DDataFromMonitor::ExtractRcInfo(KrugInfo * krug)
{
    DRcDataFromMonitor * rcinfo = (DRcDataFromMonitor *)GetPtrRcInfo();         // указатель на данные по всем РЦ
    int length = GetLenRcInfo();                                                // длина данных по всем РЦ
    COneRc *datarc = (COneRc *)(GetPtrRcInfo() + sizeof (DRcDataFromMonitor));  // указатель на данные очередной РЦ

    if (length == (int)(sizeof(DRcDataFromMonitor) + rcinfo->m_nRc * sizeof(COneRc)))
    {
        for (int i=0; i<rcinfo->m_nRc; i++, datarc++)
        {
            Rc * rc = Rc::GetById(datarc->NoRc);                                // Если многопоточное подключение - делатеся поиск РЦ с учетом круга
            if (rc == nullptr)
            {
                if (krug != nullptr)
                    qDebug() << "Не идентифицирована РЦ в потоке" << datarc->NoRc;
                continue;                                                       // не нашли - отвергаем ?
            }
            // TRACE (rc->NameEx());

            rc->stsRouteRq	= datarc->stsMRSHR_RQ;                              // состояние в уст.маршруте
            rc->stsPassed   = datarc->stsPASS;                                  // пройдена в маршруте
            rc->stsRouteOk	= datarc->stsMRSHR_OK;                              // в уст.маршруте
            rc->stsBusyFalse= datarc->stsFalseZ;                                // ложная занятость
            rc->actualRoute = datarc->Rout ? Route::GetById(datarc->Rout, krug) : nullptr;  // маршрут
            rc->actualtrain = datarc->SNo  ? Train::GetBySysNo(datarc->SNo ) : nullptr;     // поезд

            // состояние маршрута определяется "обратным ходом" по состоянию РЦ
            if (rc->actualRoute)
            {
                if (rc->stsRouteRq)
                    rc->actualRoute->sts = Route::RQSET;
                else
                if (rc->stsRouteOk && !rc->stsPassed && rc->actualRoute->sts != Route::WAIT_RZMK)
                    rc->actualRoute->sts= Route::RQOPEN;
                else
                    rc->actualRoute->sts = Route::WAIT_RZMK;
            }

            if (rc->StsBusy() && rc->actualtrain)
                rc->actualtrain->AddRc(rc);
        }
    }
    else
        TRACE ("Некорректный формат блока РЦ в потоке");
}

// TODO:
// обработка информация по поездам
void DDataFromMonitor::ExtractTrainsInfo(KrugInfo * krug)
{
    Q_UNUSED(krug)

    DTrainsDataFromMonitor * trains = (DTrainsDataFromMonitor *) GetPtrTrainsInfo();
    COneTrain * train = (COneTrain *)(GetPtrTrainsInfo() + sizeof(trains->m_nTrains));
    for (int i=0; i<trains->m_nTrains; i++)
    {
        Train::AddTrain(train->SNo, train->No);
        train++;
    }
}

