#ifndef DDATAFROMMONITOR_H
#define DDATAFROMMONITOR_H

#include "../common/defines.h"

// ==============================================================================================================================================================
//								class DDataFromMonitor
// ==============================================================================================================================================================
// Инкапсуляция данных, передаваемых по сети клиентам MONITORа,
// в частности TABLO


class DDataFromMonitor
{
#pragma pack(1)
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
    void Extract(UINT length, int bridgeno = 0);	// 2012.04.04. Передаю в функцию номер ПОТОКА (записан в номере круга)
inline WORD	IsSignatured		() { return Signature==SIGNATURE					; }
inline WORD GetLen				() { return Len										; }
    void ExtractDebugTS(/*void * pBuf,int Len*/);	// отладочная

class DStDataFromMonitor * FindStInfo(BYTE No);			// 11.02.2004. Пытаюсь обеспечить вытаскивание информации по нужным станциям
#pragma pack()
};

#endif // DDATAFROMMONITOR_H
