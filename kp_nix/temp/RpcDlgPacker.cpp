#include "stdafx.h"
#include "kp2004.h"
#include "RpcDlgPacker.h"

BYTE RpcDlgPacker::seansNo;									// счетчик сеансов

// конструктор
RpcDlgPacker::RpcDlgPacker (DWORD addr, BYTE type, BYTE * pData,WORD lenData)
{
	memset (this,0,sizeof(RpcDlgPacker));
	//ASSERT (pArr);
	marker = MARKER;
	typ = type;
	addrBM = addr;

	len	= lenData + ADDLEN;
	typ = type;
	if (lenData)
		memmove (data,pData,lenData);
	seans = ++seansNo;
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = GetCrc();
}

// Подсчет CRC и запись в пакет
WORD RpcDlgPacker::GetCrc()
{
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = 0;			// обнулим поле CRC
	WORD crc = GetCrcRPC (0xffff,(BYTE *)&len,len);			// рассчитаем с учетом поля CRC
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = crc;
	return crc;
}

// Получить значение принятой CRC
WORD RpcDlgPacker::GetCrcFromPack()
{
	return * ((WORD *)(&data[len-ADDLEN]));
}


// Разбор пакета ТС
// число ТУ, принятых от данного АРМ
BYTE RpcDlgPacker::GetAckTu1()
{
	return data[0];
}
// число ТУ, принятых от другого АРМ
BYTE RpcDlgPacker::GetAckTu2()
{
	return * GetAckTu2Ptr();
}

// число групп диагностики данного АРМ
BYTE RpcDlgPacker::GetDiag1 ()
{
	return * GetDiag1Ptr ();
}

// число групп диагностики другого АРМ
BYTE RpcDlgPacker::GetDiag2 ()
{
	return * GetDiag2Ptr ();
}

// Получить указатель на блок ТУ, принятых от данного АРМ
BYTE * RpcDlgPacker::GetAckTu1Ptr()
{
	return data;
}
// Получить указатель на блок ТУ, принятых от другого АРМ
BYTE * RpcDlgPacker::GetAckTu2Ptr()
{
	return GetAckTu1Ptr() + GetAckTu1() * TU_LEN + 1;
}
// Получить указатель на блок диагностики данного АРМ
BYTE * RpcDlgPacker::GetDiag1Ptr ()
{
	return GetAckTu2Ptr() + GetAckTu2() * TU_LEN + 1;
}
// Получить указатель на блок диагностики другого АРМ
BYTE * RpcDlgPacker::GetDiag2Ptr ()
{
	return GetDiag1Ptr () + GetDiag1 () * DIAG_LEN + 1;
}

// Получить указатель на блок ТС
WORD * RpcDlgPacker::GetTsPtr()
{
	return (WORD *) (&data [GetTuDiagBlocksLen()]);		//(GetDiag2Ptr () + GetDiag2 () * DIAG_LEN + 1);
}

// Получить общую длину блока квитанций ТУ и диагностики
WORD RpcDlgPacker::GetTuDiagBlocksLen()
{
	BYTE * p1 = GetAckTu1Ptr(),
		 * p2 = GetAckTu2Ptr(),
		 * p3 = GetDiag1Ptr (),
		 * p4 = GetDiag2Ptr ();

	int n1 = GetAckTu1(),
		n2 = GetAckTu2(),
		n3 = GetDiag1 (),
		n4 = GetDiag2 ();
	WORD l = 4 + (GetAckTu1() + GetAckTu2()) * TU_LEN
			 + (GetDiag1 () + GetDiag2 ()) * DIAG_LEN;

	return 4 + (GetAckTu1() + GetAckTu2()) * TU_LEN
			 + (GetDiag1 () + GetDiag2 ()) * DIAG_LEN;
}

// Получить число групп в пакете
WORD RpcDlgPacker::GetTsGroups()
{
	WORD l1 = GetTuDiagBlocksLen(),
		 l2 = ADDLEN;
	return (len - GetTuDiagBlocksLen() - ADDLEN) / 2;
}

// проверка корректности пакета
bool RpcDlgPacker::IsPackTsCorrect (int& error)
{
	WORD crcOrg  = GetCrcFromPack();
	WORD crcReal = GetCrc();
	if (crcOrg != GetCrc())
	{
		error = ERRCRC;
		return false;
	}
	BYTE * pEnd = (BYTE *)this + len + 1;

	if (		GetAckTu1Ptr() > pEnd
			||	GetAckTu2Ptr() > pEnd
			||	GetDiag1Ptr () > pEnd
			||	GetDiag2Ptr () > pEnd
		)
	{
		error = ERRLEN;
		return false;
	}
	if (GetTuDiagBlocksLen() + ADDLEN > len )
	{
		error = ERRLEN;
		return false;
	}
	if (typ != TYPE_TS /*&& typ != TYPE_TS_*/)
	{
		error = ERRTYPE;				// ошибка типа пакета
		return false;
	}
	return true;
}

// отобразить расшифровку пакета
// Для пакета ТС так:  6361001: 0:0:1:1 ТС=64
void RpcDlgPacker::ShowInfo(TCHAR * pBuf)
{
	if (typ==TYPE_TS /*|| typ==TYPE_TS_*/)
	{
		_stprintf( pBuf, _T("%X: %d:%d:%d:%d ТС=%d"), 
												addrBM,
												GetAckTu1(),GetAckTu2(),
												GetDiag1 (),GetDiag2 (),
												GetTsGroups());
	}
	else
	if (typ==TYPE_TU)
	{
		_stprintf( pBuf, _T("%X: ТУ:%d"),addrBM, GetTuCnt());
	}
	else
		_stprintf( pBuf, _T("ОШБ.ТИП=%02X"),typ);
}


// Получить текстовое описание по коду ошибки
LPCTSTR RpcDlgPacker::GetErrorText(int err)
{
	switch (err)
	{
		case  0		: return _T("ОК"		);
		case ERRTOUT: return _T("Тайм-аут"	);
		case ERRLEN	: return _T("Длина"	);
		case ERRCRC	: return _T("CRC"		);
		case ERRTYPE: return _T("Тип"		);
		case ERRADDR: return _T("Адрес БМ"	);
		case ERRFRMT: return _T("Формат"	);
		case ERRGBG : return _T("Мусор"		);
	}
	return _T("???");
}

/*

					НЕ НУЖНО В ВЕРСИИ КП

RpcDlgPacker::RpcDlgPacker (LPCTSTR pEsr, BYTE addr, BYTE type, BYTE * pData,WORD lenData)
{
	memset (this,0,sizeof(RpcDlgPacker));
	ASSERT (!(lenData && pData==NULL));
	marker = MARKER;
	len	= lenData + ADDLEN;
	typ = type;
	MakeAddr (pEsr,addr);
	if (lenData)
		memmove (data,pData,lenData);
	seans = ++seansNo;
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = GetCrc();
}


RpcDlgPacker::RpcDlgPacker (LPCTSTR pEsr, BYTE addr, BYTE type, CDWordArray * pArr)
{
	memset (this,0,sizeof(RpcDlgPacker));
	ASSERT (pArr);
	marker = MARKER;
	typ = type;
	MakeAddr (pEsr,addr);
	int lenData = 0;
	if (pArr)
	{
		BYTE buf[256];
		int n = min(sizeof(buf)/sizeof(WORD),pArr->GetSize());
		for (int i=0; i<n; i++)
		{
			DWORD tu = pArr->GetAt(i);
			memmove ( buf + TU_LEN * i, &tu, TU_LEN);
		}
		memmove (data,buf,n * TU_LEN);
		lenData = n * TU_LEN;
	}
	len	= lenData + ADDLEN;
	seans = ++seansNo;
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = GetCrc();
}

void RpcDlgPacker::MakeAddr (LPCTSTR(pEsr),BYTE addr)
{
	DWORD res = 0;
	for (WORD i=0; i< _tcslen (pEsr); i++)
	{
		res = res << 4;
		res += pEsr[i] - _T('0');
	}
	res = (res << 8) + addr;
	addrBM = res;
}

*/
