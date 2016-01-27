﻿#ifndef DStDataFromMonitor_H
#define DStDataFromMonitor_H

#include "enums.h"
#include "../common/defines.h"
#include "streamts.h"
#include "sysinfo.h"
#include "dras.h"

// ==============================================================================================================================================================
//								class DStDataFromMonitor
// ==============================================================================================================================================================
class DStDataFromMonitor
{
    #pragma pack(1)
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
    UINT	LastTime	[DUBL];					// Астр.время окончания последнего цикла ТС			416 / 800
	
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

    union
    {
        long    speedCom3l;                     // 4 байта полной скорости для КП2000
        BYTE    bt[4];                          // 0-скорость/1200, 1-3 байты - отказы              432 / 816
    } mvv1[2];

	WORD	RcnctCom3	[DUBL];					// число перезапусков								436 / 820
												// ? саршие 8 бит - резерв
    union
    {
        long    speedCom4l;                     // 4 байта полной скорости для КП2000
        BYTE    bt[4];                          // 0 байт - скорость/1200, 1-3 байты - отказы       440 / 824
    } mvv2[2];

	WORD	RcnctCom4	[DUBL];					// число перезапусков								448 / 832
												// ? саршие 8 бит - резерв

    UINT	tSpokSnd;							// В версиях КП до    2007 г - Время передачи данных в  ОМУЛ
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
    UINT	tSpokRcv;							// В версиях КП до    2007 г - Время приема   данных из ОМУЛ
												// В версиях КП после 2007 г - объем свободной памяти + сторона опроса + активный тракт УПОК


public:
//inline WORD GetLength() { return sizeof (DStDataFromMonitor); }
    int	 Prepare(class Station *pSt);					// сформировать данные
    class Station* Extract(class Station *st, DDataFromMonitor * pDtFrmMnt, DRas *pRas);
    class Station* Extract(class Station *st=NULL, int realTsLength = TsMaxLengthBytes, DRas *pRas = NULL);	// извлечь данные (pSt - принудительно указывает станцию)

//	void ExtractEx(DStation *pSt,int len_st, DRas *pRas=NULL);	// извлечь данные (pSt - принудительно указывает станцию)
    void ExtractDebugTS();                                  // отладочная
	void ChangeNoSt(BYTE No) { NoSt = No; }
	
    WORD GetNoSt() { return NoSt; }                         //

    void PrepareSysInfo (int i, class SysInfo* info);       // упаковать   сист.информацию
    void ExtractSysInfo (int i, class SysInfo* info);       // распаковать сист.информацию
// 11.02.2004. Пытаюсь обеспечить вытаскивание информации по нужным станциям
    #pragma pack()
};
#endif // DStDataFromMonitor_H
