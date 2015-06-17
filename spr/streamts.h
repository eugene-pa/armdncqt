#ifndef STREAMTS_H
#define STREAMTS_H

// ==============================================================================================================================================================
//								class DDataFromMonitor
// ==============================================================================================================================================================
// Инкапсуляция данных, передаваемых по сети клиентам MONITORа, 
// в частности TABLO

#include "../common/defines.h"

const int MAX_DATA_LEN_FROM_MONITOR = 65535;
//const int MIN_DATA_LEN_FROM_MONITOR = 2500;

class DDataFromMonitor
{
protected:
static BYTE DataBuf[MAX_DATA_LEN_FROM_MONITOR];
    WORD	Signature;                                      // сигнатура (55aa)
    WORD	Len;                                            // полная длина блока информации,включая сигнатуру
    WORD	nSt;                                            // число станций (ТОЛЬКО станций, число перегонов не передается, хотя инфа по ним может быть)
    WORD    LenOneSt;                                       // длина одного "подблока" информации по станции

    WORD	LenOptionsInfo;                                 // длина информации по системным настройкам (опциям)
    WORD	LenStInfo;                                      // длина информации по станциям  и перегонам >= LenOneSt * nSt
    WORD	LenRcInfo;                                      // длина информации по РЦ (модель)
    WORD	LenTrainsInfo;                                  // длина информации по поездам
//   --- блок OptionsInfo -----
//   --- блок StInfo      -----					   здесь располагается блок информации по станциям LenOneSt * nSt плюс информация по перегонам
//   --- блок RcInfo      -----
//   --- блок TrainsInfo  -----
// Принимающая просто накладывает указатель на буфер
// Далее идут собственно данные блоками указанной длины
// Блок информации по станциям включает nSt блоков 

public:	
inline WORD	GetLenOneSt			() { return LenOneSt								; }
inline WORD	GetLenOptionsInfo	() { return LenOptionsInfo							; }
inline WORD	GetLenStInfo		() { return LenStInfo								; }		
inline WORD	GetLenRcInfo		() { return LenRcInfo								; }
inline WORD	GetLenTrainsInfo	() { return LenTrainsInfo							; }

inline BYTE * GetPtrOptionsInfo	() { return ((BYTE *)this)+sizeof(DDataFromMonitor)	; }
inline BYTE * GetPtrStInfo		() { return GetPtrOptionsInfo() + LenOptionsInfo	; }
inline BYTE * GetPtrRcInfo		() { return GetPtrStInfo()		+ LenStInfo			; }
inline BYTE * GetPtrTrainsInfo	() { return GetPtrRcInfo()		+ LenRcInfo			; }

inline void	SetLenOneSt			(WORD l) { LenOneSt			=l						; }
inline void	SetLenOptionsInfo	(WORD l) { LenOptionsInfo	=l						; }
inline void	SetLenStInfo		(WORD l) { LenStInfo		=l						; }		
inline void	SetLenRcInfo		(WORD l) { LenRcInfo		=l						; }
inline void	SetLenTrainsInfo	(WORD l) { LenTrainsInfo	=l						; }

inline void PutOptionsInfo		(void *Src,WORD l) { memmove (GetPtrOptionsInfo	(),Src,l); SetLenOptionsInfo(l); } 
inline void PutStInfo			(void *Src,WORD l) { memmove (GetPtrStInfo		(),Src,l); SetLenStInfo		(l); } 
inline void PutRcInfo			(void *Src,WORD l) { memmove (GetPtrRcInfo		(),Src,l); SetLenRcInfo		(l); } 
inline void PutTrainsInfo		(void *Src,WORD l) { memmove (GetPtrTrainsInfo	(),Src,l); SetLenTrainsInfo	(l); } 

void PrepareOptionsInfo	();
void PrepareStInfo		();
void PrepareRcInfo		();
void PrepareTrainsInfo	();

bool ExtractOptionsInfo	();
void ExtractStInfo		(int BridgeNo = 0);			// 2012.04.04. Передаю в функцию номер ПОТОКА (записан в номере круга)
void ExtractStDebugTsInfo();						// отладочная
void ExtractRcInfo		(int BridgeNo = 0);			// 2012.04.04. Передаю в функцию номер ПОТОКА (записан в номере круга)
void ExtractTrainsInfo	(int BridgeNo = 0);			// 2012.04.04. Передаю в функцию номер ПОТОКА (записан в номере круга)

// Конструктор передающей стороны
//	DDataFromMonitor() { Signature = SIGNATURE; Len = sizeof (DDataFromMonitor); }
static inline DDataFromMonitor * GetObject() { return (DDataFromMonitor *)DataBuf; }
	void Prepare();
	void Extract(UINT RcvdLen, int BridgeNo = 0);	// 2012.04.04. Передаю в функцию номер ПОТОКА (записан в номере круга)
inline WORD	IsSignatured		() { return Signature==SIGNATURE					; }
inline WORD GetLen				() { return Len										; }
	void ExtractDebugTS(/*void * pBuf,int Len*/);	// отладочная

class DStDataFromMonitor * FindStInfo(BYTE No);			// 11.02.2004. Пытаюсь обеспечить вытаскивание информации по нужным станциям
};



// ==============================================================================================================================================================
//								class DPrgDataFromMonitor
// ==============================================================================================================================================================
class DPrgDataFromMonitor
{
protected:	
	WORD	Signature;							// сигнатура (55aa)
	BYTE	NoPrg;								// номер перегона
	BOOL	ChdkOn;								// вкл/окл контроль поездов по ЧДК
	short   AllEvnTrains  ;						// число поездов в четном напр.
	short   AllOddTrains  ;						// число поездов в нечетном напр.
	short   EvnTrains [MAX_TRAINS_ON_PER];		// четные поезда
	short   OddTrains [MAX_TRAINS_ON_PER];		// нечетные поезда
public:
	int	 Prepare(class DPeregon *pPrg);			// сформировать данные
	void Extract(int bridgeno);					// извлечь данные
};



// ==============================================================================================================================================================
//							class DOptionsDataFromMonitor
// ==============================================================================================================================================================

#define NEW_FORMAT_OPTIONS_DATA

class DOptionsDataFromMonitor
{
protected:
	WORD	m_ShowStrlText	 :1;				// показать надписи стрелок
	WORD	m_ShowSvtfText	 :1;				// показать надписи светофоров
	WORD	m_ShowStBox_obsolete :1;			// РЕЗЕРВ с 2015.03.18 - по факту реально не используется - показать границы станций								
	WORD	m_ShowClosedSvtf :1;				// показать закрытые светофоры
	WORD	m_ShowLockedStrl_obsolete :1;		// РЕЗЕРВ с 2015.03.18 - думаю, их надо выделять всегда, потенциальный резерв - выделить окружностью законсервированные стрелки
	WORD	m_mntrOtuOk      :1;				// норма БПДК/УПОК
	WORD    m_mntr3pages     :1;				// монитор с 3-мя страницами ТС
	WORD    m_reserved1		 :1;				// РЕЗЕРВ
	WORD    m_reserved2		 :8;				// mntrIP3	- до 2015.03.18 Использовал неявно D15-D8 mntrIP3 - оставшиеся 8 бит
	BYTE    b3;									// mntrIP4
	BYTE    b4;									// mntrVer4
// -------------------------------------------------------------------------------------------
	time_t	m_TmDt;								/// 06.09.2000. Добавил актуальное время
public:
	DOptionsDataFromMonitor () { memset (this,0, sizeof(DOptionsDataFromMonitor)); }
	int  Prepare();								//  сформировать данные
	bool Extract(UINT RcvdLen);					//  извлечь данные

// -------------------------------------------------------------------------------------------
static BYTE mntrIP3, mntrIP4, mntrVer4;
static bool mntr3pages;
static bool mntrOtuOk;
// -------------------------------------------------------------------------------------------
};


// =============================================================================
//								class DRcDataFromMonitor
// =============================================================================
// Вспомогат.класс запись на 1 РЦ
class COneRc
{
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
};
class DRcDataFromMonitor
{
public:
	WORD	m_nRc;
// Далее следует массив  COneRc[m_nRc]
};

// ==============================================================================================================================================================
//								class DTrainsDataFromMonitor
// ==============================================================================================================================================================
class COneTrain
{
public:
	short	No;
	short	SNo;
	time_t	LastTime;									// Время последней операции
};

class DTrainsDataFromMonitor
{
public:
	WORD	m_nTrains;
// Далее следует массив  COneTrain[m_nTrains]
};
#endif // STREAMTS_H

