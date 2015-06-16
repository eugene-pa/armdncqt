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
//								class DStDataFromMonitor
// ==============================================================================================================================================================
class DStDataFromMonitor
{
	friend class DDataFromMonitor;
protected:	
	WORD	Signature;							// сигнатура (55aa)									00
	BYTE	NoSt;								// номер станции									02
// -------------------------------------------------------------------------------------------------
	BYTE	reserv1;							// НОМЕР ВЕРСИИ КП вер. >= 2.0.0.186				03		
// -------------------------------------------------------------------------------------------------
	WORD	TurnOn	:1;							// [0] - Вкл / Откл									04
	WORD	Active	:1;							// [1] - актуальная станция (управляемая), А НЕ опрашивается в данный момент
	WORD	StsRu	:1;							// [2] - РУ
	WORD	StsSu	:1;							// [3] - СУ
	WORD	StsMu	:1;							// [4] - МУ
	WORD	ByByLock:1;							// [5] - логика удалений У1/У2
//	WORD	StsDu	:1;
//	WORD	StsAu	:1;
// -------------------------------------------------------------------------------------------------
	WORD    stsErrorLogic:1;					// [6] - наличие ошибки ЭЦ.ОШБ
	WORD	srsTsExpired:1;						// [7] - устарели ТС
	WORD	StsAu	:1;							// [8] - станция АУ - 2010.03.05
	WORD	BackChannel	:1;						// [9] - 2012.10.26. Признак опроса с обратного канала для КП-2000 (поле freemem используется для СПОК)
												// Резерв - 6 бит 
	// - наличие ошибки ЭЦ
	// - РСС Сетунь-2
	// - ДЦ МПК
	// - IsK7TimeExpired для ГИБРИДОВ
// -------------------------------------------------------------------------------------------------
    BYTE	TS		 [TsMaxLengthBytes];		// cостояние входов скорректированное:				06
												// TS[] = DirInput[] XOR Inverse[]
    BYTE	PulseTS	 [TsMaxLengthBytes];		// cостояние входов скорректированное:				198/390
// -------------------------------------------------------------------------------------------------
	BYTE	reserv2_1;							// 2009.12.10. адрес ЛПУ на линии					390/774		вер.2.0.0.186 (пока при приеме не используется)
	BYTE	reserv2_2;							// 2009.12.10. номер станции связи					391/775		вер.2.0.0.186 (пока при приеме не используется)
// -------------------------------------------------------------------------------------------------
	UINT	IgnoreError;						// игнорировать контроль достоверности - CONTROL_IGNORE и ЕСР
												// IgnoreError	= pSt->IgnoreError ? 0x80000000 : 0;
												// IgnoreError |= pSt->nGidUralKpId;				392/776
// -------------------------------------------------------------------------------------------------	
	BYTE	Rsrv;								// d0 - признак резервного комплекта				396/780
												// d1-d7 - резерв
	BYTE	Rsrv_EX1;							// Биты d0-d4 - ЗНАЧЕНИЕ АСТРОНОМИЧЕСКОГО ЧАСА		397/781 (начиная с 2015.03.04. вер.2.0.0.297)
												// ДЛЯ КОРРЕКТНОЙ ОБРАБОТКИ СМЕНЫ ЧАОВЫХ ПОЯСОВ при возможной их смене
												// Интересно, что для передачи данных, не привязанных к станции, получается многократное повторение данных
												// При необходимости в разных станциях можно было бы передавать разные данные
	BYTE	Rsrv_EX2;							//													398/782
	BYTE	Rsrv_EX3;							//													399/783																	
// -------------------------------------------------------------------------------------------------
	// Здесь придется передать состояние до 8 станций связи (16/2) - по 2 бита на каждую
	short	MainLineCPU;						// -1/0/1/2 (отказ/откл/WAITING/OK)					400/784
	short	RsrvLineCPU;						// -1/0/1/2 (отказ/откл/WAITING/OK)					402/768
	BYTE	LinkError	[DUBL];					// Тип ошибки: 0-OK,1-молчит,2-CRC					404/788
// -------------------------------------------------------------------------------------------------
	WORD	reserv3;							// 2009.12.10. резерв								406/790
// -------------------------------------------------------------------------------------------------
	long	CntLinkEr	[DUBL];					// Общий счетчик ошибок связи						408/792
	time_t	LastTime	[DUBL];					// Астр.время окончания последнего цикла ТС			416 / 800
	
	WORD	SysStatus	[DUBL];					// статус связи (см. LPU.C, сис.инфо)				424 / 808
												// 0 - «1» - наличие соединения на COM3 (активный модем)
												// 1 - «1» - наличие соединения на COM4 (пассивный модем)
												// 2 - «1» - ошибка инициализации модема на СОМ3
												// 3 - «1» - ошибка инициализации модема на СОМ4
												// 4 - «1» - блок программно отключился от линии (сам)
												// 5 - «1» - несовпадение ТС основного и резервного блоков
												// 6 - «1» - передача сжатого пакета обновлений ТС
												// 7 - «1» - ОМУЛ = ОК, «0» – ошибка либо отсутствие ОМУЛ
												// ? имею 8 бит запаса			

	long	SpeedCom3	[DUBL];					// 0 байт - скорость обмена/1200					428 / 812
												// 1-3 байты - отказы модулей МТС/МТУ МВВ1
	WORD	RcnctCom3	[DUBL];					// число перезапусков								436 / 820
												// ? саршие 8 бит - резерв
	long	SpeedCom4	[DUBL];					// 0 байт - скорость обмена/1200					440 / 824
												// 1-3 байты - отказы модулей МТС/МТУ МВВ2
	WORD	RcnctCom4	[DUBL];					// число перезапусков								448 / 832
												// ? саршие 8 бит - резерв

	time_t	tSpokSnd;							// В версиях КП до    2007 г - Время передачи данных в  ОМУЛ
												// В версиях КП после 2007 г:						452 / 836
												// байт 0 - GetDiag2007MVV - отказы МВВ основ
												// байт 1 - GetDiag2007MVV - отказы МВВ резерв
												// ------------------- GetDiag2007MVV ---------------
												//	0	МВВ1:   АТУ
												//	1	МВВ1:   Ош.ключа
												//	2	МВВ1:   Ош.выхода
												//	3	МВВ2:   АТУ
												//	4	МВВ2:   Ош.ключа
												//	5	МВВ2:   Ош.выхода
												//	6	"1" - АДК СЦБ = ОК (прием без ош.), 0 - ошибка или отсутствие
												//	7	"1" - АПК ДК = ОК (прием без ош.) , 0 - ошибка или отсутствие
												// --------------------------------------------------
												// байт 2 - MkuStatus      - cтатус МКУ основ
												// байт 3 - MkuStatus      - cтатус МКУ резерв
												//  ------------------ МКУ-СТАТУС ------------------
												//	0	"0" - работает (активен) основной блок,  "1" - резервный блок
												//	1	"1" - активный блок подключен к линии  
												//	2	"1" - активный блок подключен к объекту
												//	3	"1" - неактивный блок подключен к линии  
												//	4	"1" - неактивный блок подключен к объекту
												//	5	"1" - EBILOCK950=OK, 0 - ошибки нет
												//	6	"1" - РПЦ Диалог = ОК, 0 - ошибки нет 
												//	7	"1" - идет тестирование модулей
												// --------------------------------------------------	
												//													456 / 840
	time_t	tSpokRcv;							// В версиях КП до    2007 г - Время приема   данных из ОМУЛ
												// В версиях КП после 2007 г - объем свободной памяти + сторона опроса + активный тракт УПОК

	// void Extract(DStation *pSt=NULL,bool bDoShortData = false, DRas *pRas=NULL);	// извлечь данные (pSt - принудительно указывает станцию)
    void Extract(DStation *pSt=NULL,int realTsLength = TsMaxLengthBytes, class DRas *pRas=NULL);	// извлечь данные (pSt - принудительно указывает станцию)
public:
//inline WORD GetLength() { return sizeof (DStDataFromMonitor); }
	int	 Prepare(class DStation *pSt);					// сформировать данные
	
    void Extract(DStation *pSt, DDataFromMonitor * pDtFrmMnt, class DRas *pRas);

//	void ExtractEx(DStation *pSt,int len_st, DRas *pRas=NULL);	// извлечь данные (pSt - принудительно указывает станцию)
	void ExtractDebugTS();								// отладочная
	void ChangeNoSt(BYTE No) { NoSt = No; }
	
	WORD GetNoSt() { return NoSt; }						// 2010.03.29

// 11.02.2004. Пытаюсь обеспечить вытаскивание информации по нужным станциям

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


