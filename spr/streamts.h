#ifndef STREAMTS_H
#define STREAMTS_H

#include "station.h"

// ==============================================================================================================================================================
//							class DOptionsDataFromMonitor
// ==============================================================================================================================================================

#define NEW_FORMAT_OPTIONS_DATA

class DOptionsDataFromMonitor
{
#pragma pack(1)
protected:
    WORD	ShowStrlText	 :1;				// показать надписи стрелок
    WORD	ShowSvtfText	 :1;				// показать надписи светофоров
    WORD	ShowStBox_obsolete :1;              // РЕЗЕРВ с 2015.03.18 - по факту реально не используется - показать границы станций
    WORD	ShowClosedSvtf :1;                  // показать закрытые светофоры
    WORD	ShowLockedStrl_obsolete :1;         // РЕЗЕРВ с 2015.03.18 - думаю, их надо выделять всегда, потенциальный резерв - выделить окружностью законсервированные стрелки
    WORD	OtuOk          :1;                  // норма БПДК/УПОК
    WORD    large          :1;                  // монитор с 3-мя страницами ТС
    WORD    reserved1	   :1;                  // РЕЗЕРВ
    WORD    ip3            :8;                  // mntrIP3	- до 2015.03.18 Использовал неявно D15-D8 mntrIP3 - оставшиеся 8 бит
    BYTE    ip4;								// mntrIP4
    BYTE    version;							// mntrVer4
    UINT    tmdt;								// актуальное время на АРМ ДНЦ - time_t
public:
	DOptionsDataFromMonitor () { memset (this,0, sizeof(DOptionsDataFromMonitor)); }
	int  Prepare();								//  сформировать данные
    bool Extract(UINT length);					//  извлечь данные

// -------------------------------------------------------------------------------------------
static BYTE mntrIP3, mntrIP4, mntrVer4;
static bool mntr3pages;
static bool mntrOtuOk;
// -------------------------------------------------------------------------------------------
#pragma pack()
};

// ==============================================================================================================================================================
//								class DPrgDataFromMonitor
// Собственный размер класса в оригинальном виде - 31 байт, за счет отсутствия явного выравнивания pragma sizeof (DPrgDataFromMonitor) = 32
// ==============================================================================================================================================================
class DPrgDataFromMonitor
{
protected:
    WORD	Signature;							// сигнатура (55aa)
    BYTE	NoPrg;								// номер перегона
    int/*BOOL*/	ChdkOn;							// вкл/окл контроль поездов по ЧДК. ВАЖНО:  BOOL определен в defines.h как bool
    short   AllEvnTrains  ;						// число поездов в четном напр.
    short   AllOddTrains  ;						// число поездов в нечетном напр.
    short   EvnTrains [MAX_TRAINS_ON_PER];		// четные поезда
    short   OddTrains [MAX_TRAINS_ON_PER];		// нечетные поезда
public:
    int	 Prepare(class DPeregon *pPrg);			// сформировать данные
    void Extract(class KrugInfo * krug = nullptr);// извлечь данные
};


// =============================================================================
//								class DRcDataFromMonitor
// =============================================================================
// Вспомогат.класс запись на 1 РЦ
class COneRc
{
    #pragma pack(1)
public:
	WORD NoRc;
	WORD SNo;
	WORD Rout;						// актуальный маршрут
	WORD stsPASS		:1;			// уже пройденная в маршруте (не зеленая)
	WORD stsMRSHR_RQ	:1;			// устанавливаем
	WORD stsMRSHR_OK	:1;			// установлен
	WORD stsFalseZ		:1;			// Пометка ложной занятости
									// Имеем 12 бит резерва по каждой РЦ
									// Можно было бы дать занятость, замыкание, разделку, блокировку и т.д.
//-------------------------------------------------------------------	
									// 2012.06.05. Начиная с версии 2.0.0.236
	WORD stsBuzy		:1;			// Занята
	WORD stsZmk			:1;			// Замкнута
	WORD stsRzm			:1;			// Размыкание			IsTsRzm
	WORD stsLock		:1;			// Блокирована
	WORD stsMu			:1;			// Местное управление	IsTsMu
//-------------------------------------------------------------------
    #pragma pack()
};

class DRcDataFromMonitor
{
    #pragma pack(1)
public:
	WORD	m_nRc;
// Далее следует массив  COneRc[m_nRc]
    #pragma pack()
};

// ==============================================================================================================================================================
//								class DTrainsDataFromMonitor
// ==============================================================================================================================================================
class COneTrain
{
    #pragma pack(1)
public:
	short	No;
	short	SNo;
    UINT	LastTime;									// Время последней операции - time_t
    #pragma pack()
};

class DTrainsDataFromMonitor
{
    #pragma pack(1)
public:
	WORD	m_nTrains;
// Далее следует массив  COneTrain[m_nTrains]
    #pragma pack()
};
#endif // STREAMTS_H

