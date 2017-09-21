// Rpc.h: interface for the CRpc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPC_H__13DC5526_6FA1_42E6_B895_66CA87C61D6D__INCLUDED_)
#define AFX_RPC_H__13DC5526_6FA1_42E6_B895_66CA87C61D6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


const DWORD DefaultBetweenTu = 500;

//11.03.2011 ВНИМАНИЕ.Незя объявлять виртуальные ф-ии, т.к.смещаются адреса переменных
class CRpc : public CRS232  
{
public:
	CRpc(CWnd* pWnd,LPCTSTR Port,LPCTSTR szInit);
	~CRpc();

static CDWordArray arAddrBmEx;			// адреса БМ, имеющие место быть; заполняется при чтении INI файла
static CWordArray arTsGroups;			// массив числа групп ТС, индексированный по номеру БМ 
static CWordArray arTsOffset;			// массив смещений групп ТС, индексированный по номеру БМ 
static CDWordArray  arTimeLast;

	UINT	nRcvBt,nRcvPk,				// public счетчики приема
			nSntBt,nSntPk,				// public счетчики передачи
			nErrors;					// public счетчик  ошибок
	int		rpcLastError;				// код последней ошибки RPC или 0
	bool	bShowDetail;				// флаг отсылки всех сообщений состояния бмена

	void PollingLine();
	void ClearCounters () { nRcvBt = nRcvPk = nSntBt = nSntPk = nErrors = 0; }
	//04.03.2011 Изменил	
	bool IsTout();
	inline bool IsOK() { return !IsTout(); }

	void StrRpcTu (WORD tu, LPTSTR buf, int buflen);

	DWORD	tickLastTuSent;					// время в тиках последней отправки ТУ
											// используется, чтобы гарантировать паузу между двумя ТУ 0.5 сек.
	DWORD   tickBetweenTu;					// задержка между командами ТУ. По умолчанию 500 мс, может меняться
											// на один такт виртуальной командой ПАУЗА (модуль = 0, выход > 1)
	int		nGroupTu;						// признак группировки n следующих ТУ в один пакет
	
	bool	bNextTuSolo;					// фоаг, взводисый виртуальной командой "С ВЫДЕРЖКОЙ"
											// (до окончания след. команды блокировать ТУ)
	bool	bOtu1;							// фоаг, взводисый виртуальной командой "РПЦ-ОТУ-1"
	bool	bOtu2;							// фоаг, взводисый виртуальной командой "РПЦ-ОТУ-2"

	BYTE    bmMask;							// маска адресации БМ

	// преобразования ТУ в компоненты модуль, выход, время
	void MakeRpcDlgTu (WORD tu, BYTE * ptr);
	BYTE rpcGetOutFromTu  (WORD tu) { return tu & 0x3f;	} // выделить номер выхода из кода IJ (биты d5-d0
	BYTE rpcGetMdlFromTu  (WORD tu);		// выделение номера модуля в формате НСИ ДЦ из кода IJ (биты d10-d6)
	BYTE rpcGetDelpcFromTu(WORD tu);		// выделить задержку ТУ из команды ТУ (в 100мс единицах)
	BYTE rpcGetFormatMdl(BYTE m);			// форматировать модуль для передачи в РПЦ из номера модуля в НСИ ДЦ
	BYTE rpcGetProektMdl(BYTE m);			// получить проектный номер модуля по модуля в БД НСИ

protected:
	int  GetIndxBm(DWORD addr);			// Определить индекс (порядковый номер БМ) в списке arAddrBmEx по адресу
	BOOL GetDataFromLine();
	BOOL ExtractTs();	
	bool GetMarker();
	bool GetRsByteRpc(CRS232* pRS,BYTE* pByte);
#if  defined(DEF_RPC_SIMULATE)
	bool GetRsByteRpcSimulate(CRS232* pRS,BYTE* pByte);
#endif
	//09.03.2011 След.ф-ии добавлены
	//void SendAckErr(int err);
	//bool udpLogRpc (TCHAR * ptr,bool bPrmPrd);
	//bool udpLogRpc (BYTE *ptr, int len,bool bPrmPrd);
	bool udpLogHexRpc (BYTE *ptr, int len,bool bPrmPrd);
	void PutToArchive(PBYTE pBuf,int Len,bool bPrmPrd);

	int		IndxPolling;					// индекс поллинга в массиве адресов БМ
	BYTE	bufIn [1024];
	int		LenIn;							// длина принятого пакета
	//time_t	m_TimeLastRecv;					// массив временных точек для каждого БМ
	bool	m_bAckTout;

};

#endif // !defined(AFX_RPC_H__13DC5526_6FA1_42E6_B895_66CA87C61D6D__INCLUDED_)
