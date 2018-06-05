#ifndef STATIONNETTS_H
#define STATIONNETTS_H

#include "../common/defines.h"

// =============================================================================
//								class StationNetTS
// =============================================================================
// класс для передачи данных от станции связи в MONITOR
// 2012.10.26. Мы имеем достаточно много свободного места в этом пакете, в принципе, можно было бы передавать доп.диагностическую информацию
// например: реальная частота опроса станции
//           последние значения длин инфо-блоков
//
// ВАЖНО:
// 2018.05.28. Введена pragma(1) и типы данных изменены для совместимости MSVC - QT по размерам
//
#pragma pack(1)
class StationNetTS
{
public:
    WORD	signature;							// сигнатура (55aa)									00
    WORD	length;								// полная длина сетевого блока включая сигнатуру	02
    short	mainLineCPU;						// -1/0/1/2 (отказ/откл/WAITING/OK)					04
    short	rsrvLineCPU;						// -1/0/1/2 (отказ/откл/WAITING/OK)					06
    BYTE	nost;								// номер станции									08
    BYTE    reserv1;							// зарезервировано 24 бита
    BYTE    reserv2;							// МОЖНО И НУЖНО ПЕРЕДАТЬ НОМЕР ВЕРСИИ СТАНЦИИ СВЯЗИ, МОЖНО И НУЖНО ПЕРЕДАТЬ IP-АДРЕС СТАНЦИИ СВЯЗИ
    BYTE    reserv3;

    DWORD	rsrv		:1;						// станция на резерве								0c
    DWORD	backChannel	:1;						// опрос по обводному каналу	- 2012.10.25 - задействован еще 1 бит
    DWORD	reserv4		:30;					// зарезервировано 30 бит		- 2012.10.25 - отсалось 30 бит


    BYTE	seans;								// номер сеанса связи								10
    BYTE    reserv5;							// зарезервировано 24 бита
    BYTE    reserv6;
    BYTE    reserv7;

// далее переменные представлены для основного[0] и резервного[1] каркасов
    DWORD	bypassSts	[DUBL];					// состояние байпаса:								14	- НЕ ИСПОЛЬЗУЕТСЯ !!!
    int     lastTime	[DUBL];					// Астр.время окончания последнего цикла ТС			1с      time_t
    int     oldTime		[DUBL];					// Астр.время окончания i-1 цикла ТС				24      time_t
    WORD	linkError	[DUBL];					// Тип ошибки: 0-OK,1-молчит,2-CRC					2с
    int     cntLinkEr	[DUBL];					// Общий счетчик ошибок связи						30
    WORD	linkTime	[DUBL];					// длительность сеанса								38
    WORD	realDataLen;						// длина блока данных из линии						3c
    BYTE	inputData[MAX_LINE_DATA_LEN];		// собственно данные из линии в оригинальном виде	3e
    WORD    ext;                                // выравнивание для совместимости

    StationNetTS();                             // Конструктор
    StationNetTS(class Station * st);           // Конструктор
    int  Pack (class Station   * st);           // ормирование данных

    void SetLenByDataLen(WORD datalength);
inline static int MinSizeof()					// 2014.03.20.вер.2.0.0.277. Минимально допустимый размер пакета
    { return sizeof (StationNetTS) - sizeof (inputData); }
};
#pragma pack()
#endif // #ifndef STATIONNETTS_H
