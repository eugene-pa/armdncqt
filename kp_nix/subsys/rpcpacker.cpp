#include <cstring>
#include "rpcpacker.h"

BYTE RpcPacker::seansNo;									// счетчик сеансов

// конструктор
RpcPacker::RpcPacker (DWORD addr, BYTE type, BYTE * pData,WORD lenData)
{
    std::memset (this, 0, sizeof(RpcPacker));
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
WORD RpcPacker::GetCrc()
{
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = 0;			// обнулим поле CRC
	WORD crc = GetCrcRPC (0xffff,(BYTE *)&len,len);			// рассчитаем с учетом поля CRC
	* ((WORD *)(((BYTE *)this) + len +1 - 2)) = crc;
	return crc;
}

// Получить значение принятой CRC
WORD RpcPacker::GetCrcFromPack()
{
	return * ((WORD *)(&data[len-ADDLEN]));
}


// Разбор пакета ТС
// число ТУ, принятых от данного АРМ
BYTE RpcPacker::GetAckTu1()
{
	return data[0];
}
// число ТУ, принятых от другого АРМ
BYTE RpcPacker::GetAckTu2()
{
	return * GetAckTu2Ptr();
}

// число групп диагностики данного АРМ
BYTE RpcPacker::GetDiag1 ()
{
	return * GetDiag1Ptr ();
}

// число групп диагностики другого АРМ
BYTE RpcPacker::GetDiag2 ()
{
	return * GetDiag2Ptr ();
}

// Получить указатель на блок ТУ, принятых от данного АРМ
BYTE * RpcPacker::GetAckTu1Ptr()
{
	return data;
}
// Получить указатель на блок ТУ, принятых от другого АРМ
BYTE * RpcPacker::GetAckTu2Ptr()
{
	return GetAckTu1Ptr() + GetAckTu1() * TU_LEN + 1;
}
// Получить указатель на блок диагностики данного АРМ
BYTE * RpcPacker::GetDiag1Ptr ()
{
	return GetAckTu2Ptr() + GetAckTu2() * TU_LEN + 1;
}
// Получить указатель на блок диагностики другого АРМ
BYTE * RpcPacker::GetDiag2Ptr ()
{
	return GetDiag1Ptr () + GetDiag1 () * DIAG_LEN + 1;
}

// Получить указатель на блок ТС
WORD * RpcPacker::GetTsPtr()
{
	return (WORD *) (&data [GetTuDiagBlocksLen()]);		//(GetDiag2Ptr () + GetDiag2 () * DIAG_LEN + 1);
}

// Получить общую длину блока квитанций ТУ и диагностики
WORD RpcPacker::GetTuDiagBlocksLen()
{
//	BYTE * p1 = GetAckTu1Ptr(),
//		 * p2 = GetAckTu2Ptr(),
//		 * p3 = GetDiag1Ptr (),
//		 * p4 = GetDiag2Ptr ();

//	int n1 = GetAckTu1(),
//		n2 = GetAckTu2(),
//		n3 = GetDiag1 (),
//		n4 = GetDiag2 ();
//	WORD l = 4 + (GetAckTu1() + GetAckTu2()) * TU_LEN
//			 + (GetDiag1 () + GetDiag2 ()) * DIAG_LEN;

	return 4 + (GetAckTu1() + GetAckTu2()) * TU_LEN
			 + (GetDiag1 () + GetDiag2 ()) * DIAG_LEN;
}

// Получить число групп в пакете
WORD RpcPacker::GetTsGroups()
{
	WORD l1 = GetTuDiagBlocksLen(),
		 l2 = ADDLEN;
	return (len - GetTuDiagBlocksLen() - ADDLEN) / 2;
}

// проверка корректности пакета
bool RpcPacker::IsPackTsCorrect (int& error)
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
std::wstring RpcPacker::ShowInfo()
{
    std::wstringstream tmp;

    if (typ==TYPE_TS )  // || typ==TYPE_TS_
        tmp << std::hex << addrBM << std::dec << L": " << GetAckTu1() << L":" << GetAckTu2() << L":" << GetDiag1 ()<< L":" << GetDiag2 ()<< L":" << GetTsGroups();
	else
	if (typ==TYPE_TU)
        tmp << std::hex << addrBM << std::dec << L": ТУ:" << GetTuCnt();
	else
        tmp << L"ОШБ.ТИП=" << std::hex << typ;
    return tmp.str();
}



// Получить текстовое описание по коду ошибки
std::wstring RpcPacker::GetErrorText(int err)
{
	switch (err)
	{
        case  0		: return L"ОК"		;
        case ERRTOUT: return L"Тайм-аут";
        case ERRLEN	: return L"Длина"	;
        case ERRCRC	: return L"CRC"		;
        case ERRTYPE: return L"Тип"		;
        case ERRADDR: return L"Адрес БМ";
        case ERRFRMT: return L"Формат"	;
        case ERRGBG : return L"Мусор"	;
	}
    return L"???";
}

std::wstring RpcPacker::VirtTuText(VIRT_TU vtu)
{
    switch (vtu)
    {
        case VRT_CLEAR     : return L"РПЦ.Сброс режима виртуальной команды по тайм-ауту"     ;
        case VRT_GROUP     : return L"РПЦ.ВИРТ: ГРУППА ТУ"                                   ;
        case VRT_SOLO      : return L"РПЦ.ВИРТ: С ВЫДЕРЖКОЙ"                                 ;
        case VRT_OTU1      : return L"РПЦ.ВИРТ: РПЦ-ОТУ-1"                                   ;
        case VRT_OTU2      : return L"РПЦ.ВИРТ: РПЦ-ОТУ-2"                                   ;
        case VRT_PAUSE     : return L"РПЦ.ПАУЗА "                                            ;
        case VRT_WAITING   : return L"РПЦ.ТУ выполняется в изолированном режиме с выдержкой ";
        case VRT_BM1	   : return L"РПЦ.ВИРТ: вывод ТУ на БМ1 "				             ;
        case VRT_BM2       : return L"РПЦ.ВИРТ: вывод ТУ на БМ2 "				             ;
    }
    return L"???";
}

