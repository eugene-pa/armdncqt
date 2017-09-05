#ifndef DEF_RpcDlgPacker

extern WORD GetCrcRPC( WORD wCrc, BYTE* pBuf, int nSize);

#define DEF_RpcDlgPacker
// Класс-упаковщик блока трехбайтных инструкций
class CmdPacker
{
public:
	enum
	{
		MAX_CMD = 16,
	};
	DWORD GetCmd(BYTE i) { return i < min(cnt,MAX_CMD) ? info[i][0] + info[i][1]*256 + info[i][2]*256 : 0; }
#pragma pack(1)	
	BYTE cnt;
	BYTE info[16][3];
#pragma pack()	
};


class RpcDlgPacker
{
public:
	RpcDlgPacker();
//	RpcDlgPacker (LPCTSTR pEsr, BYTE addr, BYTE type, BYTE * pData=NULL,WORD lenData=0);
//	RpcDlgPacker (LPCTSTR pEsr, BYTE addr, BYTE type, CDWordArray * pArr);
//	RpcDlgPacker (DWORD addr, BYTE type, CDWordArray * pArr);
	RpcDlgPacker (DWORD addr, BYTE type, BYTE * pData=NULL,WORD lenData=0);

	WORD GetCrc();
	WORD GetCrcFromPack();
	WORD GetFullLen() { return len + 1; }
	
	// Разбор пакета ТС
	BYTE GetAckTu1();				// число ТУ, принятых от данного АРМ
	BYTE GetAckTu2();				// число ТУ, принятых от другого АРМ
	BYTE GetDiag1 ();				// число групп диагностики данного АРМ
	BYTE GetDiag2 ();				// число групп диагностики другого АРМ

	BYTE * GetAckTu1Ptr();
	BYTE * GetAckTu2Ptr();
	BYTE * GetDiag1Ptr ();
	BYTE * GetDiag2Ptr ();
	
	WORD GetTuDiagBlocksLen();
	WORD * GetTsPtr();				// Получить указатель на блок ТС
	WORD GetTsGroups();
	
	WORD GetTuCnt() { return (len - ADDLEN) / TU_LEN; }
	
	bool IsPackTsCorrect (int& error);// проверка корректности пакета
	void ShowInfo(TCHAR * pBuf);	// отобразить расшифровку пакета

	enum
	{
		ERRTOUT	= -1,				// ошибка тайм-аута
		ERRLEN	= -2,				// ошибка длины
		ERRCRC	= -3,				// ошибка CRC
		ERRTYPE	= -4,				// ошибка типа пакета
		ERRADDR	= -5,				// ошибка адреса БМ - нет такого адреса
		ERRFRMT	= -6,				// ошибка формата
		ERRGBG	= -7,				// ошибка мусора

		TU_LEN  = 3,				// длина команды ТУ в байтах
		DIAG_LEN= 3,				// длина команды ТУ в байтах
		ADDLEN	= 10,				// длина пакета, добавляемая к длине данных
		MARKER	= 0xDB,				// маркер начала пакета
		TYPE_TU = 0x86/*8E*/,		// тип пакета ТУ от КП в БМ
		
		TYPE_TS = 0x06/*0E*/,		// тип пакета ТС от БМ в КП
//		TYPE_TS_= 0x06,				// тип пакета ТС по пакету Сергея

		MAX_LEN = 512,
	};
//	void MakeAddr (LPCTSTR(pEsr),BYTE addr);
static LPCTSTR GetErrorText(int err);
static bool CheckAnyLen(WORD l) { return l >= ADDLEN && l < MAX_LEN + ADDLEN; }
static BYTE seansNo;
//protected:
#pragma pack(1)	
	BYTE	marker;					// маркер 0xDB
	WORD	len;					// длина 
	BYTE	typ;					// тип пакета (TYPE_TU = 0x8E, TYPE_TS = 0x0E)
	BYTE	seans;					// сеанс
	DWORD	addrBM;					// адрес БМ
	BYTE	data[MAX_LEN];			// данные и CRC
//	WORD	crc;
#pragma pack()	
};

#endif // #ifndef RpcDlgPacker
