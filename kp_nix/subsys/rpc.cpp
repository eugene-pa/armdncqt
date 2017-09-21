#include <thread>
#include "common/common.h"
#include "common/pamessage.h"
#include "common/tu.h"
#include "rpc.h"
#include "rpcpacker.h"

// Реализация класса RpcBM и протокола обмена КП с РПЦ Диалог
// Интерфейс - RS-485, РПЦ может включать до 4-х БМ.
// Каждая БМ описывается в конфигурации отдельно и представлена в ПО КП в виде класса RpcBM

// ---------------------------------------------------------------------------------------------------------------------------
// члены класса RpcBM
std::vector<RpcBM> RpcBM::allBM;            // массив всех БМ станции
int RpcBM::allGroups;                       // обще число групп на все БМ

RpcBM::RpcBM(DWORD addr, int n)             // конструктор, принимающий уникальный адрес БМ и число групп ТС
{
    addrBM = addr;                          // адрес
    groups = n;                             // число групп
    offset = allGroups;                     // offset - сумма предыдущих групп
    allGroups += groups;                    // подсчитываем сумму длин групп

    allBM.push_back(*this);                 // добавляем класс в массив (вектор) БМ
}
// ----------------------------------------------------------------------------------------------------------------------

// Реализация протокола
std::queue <std::shared_ptr<Tu>> rpcToDo;   // очередь ТУ РПЦ на исполнение
std::mutex rpc_lock;                        // блокировка доступа к очереди rpcToDo

// Для хранения данных РПЦ (ТС, диагностика) используем глоб.переменные
// (нет смысла делать их членами класса RpcBM)
BYTE TsStatusRpc   [RpcPacker::MAX_LEN];	// массив состояний ТС(РПЦ)
WORD TsRpcSize = 0;							// размер суммарных данных в блоке ТС

// вспомогательные переменные для реализации виртуальных ТУ РПЦ
int     nGroupTu	= 0;                    // счетчик группировки n следующих ТУ в один пакет
bool    bNextTuSolo	= false;				// блокировать последующие ТУ до окончания последней команды
bool    bOtu1		= false;				// флаг, взводисый виртуальной командой "РПЦ-ОТУ-1"
bool    bOtu2		= false;				// флаг, взводисый виртуальной командой "РПЦ-ОТУ-2"
BYTE    bmMask		= 0;                    // маска адресации БМ

// Далее: массив состояний Диагностики РПЦ. Реальная длина DiagRpcSize = 4 + 6 * число_шкафов
// Первые 4 байта отводятся для информации по работающим БМ в шкафах (макс. - 4 шкафа)
//        В них храним используем A7,A6 - номер БМ, от которой получили
// Следующие 6 * nБМ байт - по 6 байт первых диагностических групп по каждому шкафу
//        Реально диагностические группы у нас нигде не привязаны
const int ourDiagLen = 4;					// резервируем 4 байта под 4 шкафа всегда
BYTE DiagStatusRpc [RpcPacker::MAX_LEN];	// буфер накопления диагностики
WORD DiagRpcSize = ourDiagLen;				// здесь считаем реальный общий размер блока Диагностики РП 4 + 6 * число_шкафов

const int DefaultBetweenTu = 500;           // задержка между ТУ по умолчанию
std::chrono::time_point<std::chrono::system_clock> tickLastTuSent;  // засечка времени последней отправки ТУ
int  tickBetweenTu	= DefaultBetweenTu;     // регулируемая задержка между командами ТУ (по умолчанию 500 мс)

// число команд в очереди на исполнение РПЦ
int HowManyToDo()
{
    std::lock_guard <std::mutex> locker(rpc_lock); // блокировка rpc_lock
    return (int)rpcToDo.size();
}

// проверка истечения заданного интервала времени между ТУ
bool IsTimeToTU()
{
    return std::chrono::system_clock::now() > tickLastTuSent + std::chrono::milliseconds(tickBetweenTu);
}

// получить, если есть, очередную команду РПЦ для исполнения с извлечением из очереди РПЦ
// если очередь пуста - возвращаем nullptr
std::shared_ptr<Tu> GetTuRpc()
{
    std::shared_ptr<Tu> tu = nullptr;
    std::lock_guard <std::mutex> locker(rpc_lock); // блокировка rpc_lock
    if (rpcToDo.size() > 0)
    {
        tu = rpcToDo.front();
        rpcToDo.pop();
    }
    return tu;
}

// 2013.07.16. выделение номера модуля в формате НСИ ДЦ из кода IJ (биты d10-d6)
BYTE rpcGetMdlFromTu  (WORD tu)
{												// TTTT T
    return (tu & 0x07c0) >> 6;					// ****.*МММ.ММ**.**** - берем только 5 бит, старший - под старший бит задержки
}												// fedc.ba98.7654.3210

// выделить номер выхода из кода IJ (биты d5-d0)
BYTE rpcGetOutFromTu  (WORD tu)
{
    return tu & 0x3f;
}

// выделить задержку ТУ из команды ТУ (в 100мс единицах)
// 2013.07.16. Бит d11 используется для расширения разрядности задержки в качестве СТАРШЕГО бита
//             Таким образом под задержку тспользуется 5 битов с шагом 500мс.Максимальная задержка = 15,5 сек
BYTE rpcGetDelpcFromTu(WORD tu)
{
    BYTE delay = (tu & 0xf000) >> 12;			// выделяем младшие 4 бита задержки
    if (tu & 0x0800)							// если установлен бит d11
        delay |= 0x10;							// добавляем старший бит задержки
    return (delay * 10 ) / 2;					// преобразуем единицы измерения 0.5 сек в 100мс.
}

void RpcBM::PollingLine(BlockingRS& rs)             // реализация опроса
{
//     if ( !KP.IsControlOne() )                    // если не на линии - выход
//        return;

    // опрос всех БМ
    for (auto bm : allBM)
    {
        DWORD addr = bm.addrBM;

        if (HowManyToDo() && IsTimeToTU())          // есть команды ТУ    ?
        {
            std::shared_ptr<Tu> tu = GetTuRpc();

            if ( tu==nullptr || tu->GetTu()==0 )    // команда с кодом 00 - недопустимая; игннорируем
            {
                bNextTuSolo = bOtu1 = bOtu2 = false;// сброс флагов вирт.ТУ
                bmMask   = 0;
                nGroupTu = 0;
                Log(RpcPacker::VirtTuText(RpcPacker::VRT_CLEAR));
                break;
            }

            SendMessage(new PaMessage(tuAckToDo, tu));  // принята к исполнению
/*
            // вычисляем номер модуля и отдельно отрабатываем виртуальные команды с номером модуля = 0
            BYTE noMdl = rpcGetMdlFromTu (tu->GetTu());
            BYTE noOut;
            // ======================================================================================
            if (noMdl == 0)
            {
                // 1. Обработать виртуальные команды
                //	ВЫХОД = 1    - "ГРУППА ТУ"   две следующие команды в один пакет
                //	ВЫХОД = 2    - "С ВЫДЕРЖКОЙ" до окончания исполнения след.команды ТУ блокируются
                //	ВЫХОД = 3    - "РПЦ-ОТУ-1"
                //	ВЫХОД = 4	 - "РПЦ-ОТУ-2"
                //  ВЫХОД = 5	 - "ТУБМ1"		 след.ТУ выдается на БМ1
                //  ВЫХОД = 6	 - "ТУБМ2"		 след.ТУ выдается на БМ2
                //	ВЫХОД = 11...- "ПАУЗАx"      задержка в цикле выдачи ТУ на указанное время

                noOut = rpcGetOutFromTu (tu->GetTu());
                // МОДУЛЬ = 0, ВЫХОД = ВИРТ
                switch (noOut)
                {
                    case 1:									// 0:1  "ГРУППА ТУ"
                    {
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_GROUP));
                        nGroupTu = 1;						// если nGroupTu >0 - не выходим после Make
                        continue;
                    }
                    case 2:									// 0:2  "С ВЫДЕРЖКОЙ"
                    {
                        bNextTuSolo = true;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_SOLO));
                        continue;
                    }
                    case 3:									// 0:3  "РПЦ-ОТУ-1"
                    {
                        bOtu1 = true;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_OTU1));
                        continue;
                    }
                    case 4:									// 0:4  "РПЦ-ОТУ-2"
                    {
                        bOtu2 = true;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_OTU2));
                        continue;
                    }
                    case 5:									// 0:5 "ТУБМ1"
                    {
                        bmMask = 0x40;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_BM1));
                        continue;
                    }
                    case 6:									// 0:6 "ТУБМ2"
                    {
                        bmMask = 0x80;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_BM2));
                        continue;
                    }
                    // МОДУЛЬ = 0, ВЫХОД > 1 - задержка в цикле выдачи ТУ на указанное время
                    default:								// 0:3 и т.д. - "ПАУЗА"
                    {
                        tickBetweenTu = rpcGetDelpcFromTu(tu->GetTu()) * 100;
                        Log(RpcPacker::VirtTuText(RpcPacker::VRT_PAUSE));
                        break;
                    }
                }
            }
*/
            SleepMS(500);										// задержка на исполнение ТУ
            SendMessage(new PaMessage(tuAckDone, tu));          // принята исполнена

        }
    }
}



// поток взаимодействия с РПЦ Lbfkju
void ThreadRpc(long param)
{
    std::wstringstream s;
    s << L"Поток опроса РПЦ Диалог запущен. threadid=" << std::this_thread::get_id();
    SendMessage(new PaMessage(s.str()));

    QString config = QString::fromStdWString(*(std::wstring *)param);
    BlockingRS rs(config);
    rs.start();

    // Циклическая связь с РПЦ вплоть до запроса выхода (освобождение exit_lock)
    while (!exit_lock.try_lock_for(chronoMS(100)))
    {
        RpcBM::PollingLine(rs);
    }
    exit_lock.unlock();
    rs.Close();

    s.str(std::wstring());
    s << L"Поток опроса опроса РПЦ Диалог завершен. threadid=" << std::this_thread::get_id();
    Log(s.str());
}

