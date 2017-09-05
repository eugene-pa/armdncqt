// Common.cpp - функции общего назначения
#include "stdafx.h"
#include "kp2004.h"
#include "ReadRegistry.h"

// Функция получения байта из заданного порта и записи по заданному адресу
// Вход: 
// pRS	- указатель на RS-класс
// pByte- адрес назначения для байта
// Выход: истина - если байт считан
//		  ложь   - ошибка, либо запрос на выход
bool GetRsByte(CRS232* pRS,BYTE* pByte)
{
	int ch = pRS->GetChar();
	if (ch<0 || bExitPending) return false;
	*pByte = (BYTE) ch;
	return true;
}

// Побайтовый алгоритм вычисления CRC
// (Р.Л.Хаммел,"Послед.передача данных", Стр.49. М.,Мир, 1996)
const WORD CRC_POLY = 0x1021;
WORD GetCRC (BYTE *PackToSend,WORD Len,WORD wInit)
{
	WORD j,w,Crc = wInit;
	while (Len--)
	{
		w = (WORD)(*PackToSend++)<<8;
		Crc ^= w;
		for (j=0; j<8; j++)
			if (Crc & 0x8000)
				Crc = (Crc<<1) ^ CRC_POLY;
			else
				Crc = Crc << 1;
	}
	return Crc;
}

/*
  Алгоритм CRC32 основан на примитивном полиноме EDB88320 (шестн).
  http://dic.academic.ru/dic.nsf/ruwiki/9765

  Name  : CRC-32
  Poly  : 0x04C11DB7	x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение одинарных, двойных, пакетных и всех нечетных ошибок
*/
unsigned long GetCRC32(unsigned char *buf, unsigned long len)
{
    unsigned long crc_table[256];
    unsigned long crc;
 
    for (int i = 0; i < 256; i++)
    {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
 
        crc_table[i] = crc;
    };
 
    crc = 0xFFFFFFFFUL;
 
    while (len--) 
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
 
    return crc ^ 0xFFFFFFFFUL;
};


// Таймаут на заданное время с контролем глобального события завершения приложения
int ControlSleep (int nTimeout)
{
	return WaitForSingleObject(CRS232::ghThreadExitEvent,nTimeout);
}


#ifdef DEBUG_CROSS_IN_OUT
// буфер для хранения  эмулируемого состояния регистров в процессе отладки 
// позволяет задавать в процессе работы состояние входов выводом в соотв.порт
//22.05.2009 
BYTE DebugPorts[0x1000];				
#endif // #ifdef DEBUG_CROSS_IN_OUT

// Ввод данных
int  inp (WORD addr)
{
	WORD wRet;
	_asm
	{
		mov		dx,addr
		mov		ax,0
		in		al,dx
		mov		wRet,ax
	}
	
//	TRACE (_T("%x --> %02x\n"),addr, wRet);
#ifdef DEBUG_CROSS_IN_OUT
	wRet = DebugPorts[addr];
#endif // #ifdef DEBUG_CROSS_IN_OUT
	return wRet;
}


// Вывод данных
void outp(WORD addr,WORD data)
{
	_asm
	{
		mov		dx,addr
		mov		ax,data
		out		dx,al
	}

//	TRACE (_T("%x <-- %02x\n"),addr, data);
#ifdef DEBUG_CROSS_IN_OUT
	DebugPorts[addr] = (BYTE)data;
#endif // #ifdef DEBUG_CROSS_IN_OUT

#ifdef DEF_TU_STEP
	CString s;
	s.Format(_T("%x <-- %02d\n"),addr, data);
	AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(LPARAM)(LPCTSTR)s);
	Sleep(2000);
#endif //#ifdef DEF_TU_STEP

}

//Меняем аттрибуты некоторых файлов на системные во избежание их удаления
bool ChangeFileAttrib()
{
	LPCTSTR sFile[] = 
	{
#ifdef DEF_EMULATOR
		_T("\\*.sys"  ),
		_T("\\command.com" ),
		_T("\\autoexec.bat"),
		_T("\\start.bat"),
		_T("\\loadcepc.exe"),
		_T("\\nk*.*")
#else
		_T("\\NAND_Flash\\config.sys"  ),
		_T("\\NAND_Flash\\command.com" ),
		_T("\\NAND_Flash\\autoexec.bat"),
		_T("\\NAND_Flash\\start.bat"),
		_T("\\NAND_Flash\\loadcepc.exe"),
		_T("\\NAND_Flash\\nk*.*")
#endif
	};

	bool bRet = false;
	BOOL bRet1 = false;
	HANDLE hFile = NULL;
	WIN32_FIND_DATA fd;
	DWORD dwAtrib = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY;
	TCHAR buf[256];
	for( int i=0; i<sizeof(sFile)/sizeof(LPCTSTR); i++ )
	{
		hFile = FindFirstFile(sFile[i], &fd);
		if( hFile != INVALID_HANDLE_VALUE )
		{
			bRet1 = TRUE;
			while( bRet1 )
			{
				if( (fd.dwFileAttributes & dwAtrib) != dwAtrib )
				{
#ifdef DEF_EMULATOR
					_stprintf(buf,_T("\\%s"),fd.cFileName);
#else
					_stprintf(buf,_T("\\NAND_Flash\\%s"),fd.cFileName);
#endif
					if( SetFileAttributes(buf,fd.dwFileAttributes|dwAtrib) )
					{
						AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(LPARAM)buf);
						bRet = true;
					}
				}
				bRet1 = FindNextFile(hFile, &fd);
			}
			FindClose(hFile);
		}
	}

	return bRet;
}

// Изменить приоритет потока на зад.кол-во уровней
// pThread	- указатель на поток
// d		- число единиц со знаком 
// Возвращает новый приоритет или -1 в случае ошибки
// ПРИМЕЧАНИЕ: отрицательный знак повышает реальный приоритет, так как 0 - высший приоритет
int ChangeThreadPriority ( CWinThread *pThread, int d )
{
	int pr = -1;
	if ( pThread && pThread->m_hThread )
	{
		//30.07.2014 Ф-ии другие
		//pr = pThread->CeGetThreadPriority();
		//pThread->CeSetThreadPriority(pr + d);
		//pr = pThread->CeGetThreadPriority();
		pr = pThread->GetThreadPriority();
		pThread->SetThreadPriority(pr + d);
		pr = pThread->GetThreadPriority();
	}
	return pr;
}

//Установить время
//Вход: tm - сек.,время в формате DWORD, полученное ф-ей GetTime()
bool SetLocalTime_kp2004(DWORD tm)
{
	CTime t(tm);
	SYSTEMTIME st;
	st.wYear		= t.GetYear(); 
	st.wMonth		= t.GetMonth(); 
	st.wDayOfWeek	= t.GetDayOfWeek();
	st.wDay			= t.GetDay(); 
	st.wHour		= t.GetHour(); 
	st.wMinute		= t.GetMinute(); 
	st.wSecond		= t.GetSecond(); 
	st.wMilliseconds= 500; 

//TCHAR buf[256];
//_stprintf(buf,_T("Синхронизация времени с УПОК: %.2d:%.2d:%.2d %.2d.%.2d.%.4d"),
//			st.wHour,st.wMinute,st.wSecond, st.wDay, st.wMonth, st.wYear );
//AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(long)buf);

	return SetLocalTime(&st) ? true : false;
}

//Пребразование hex-строки в байтовый hex-массив
//Вход:
// - sHex	- hex-строка вида: "01 02 aa 1b"
// - pBuf	- массив
// - czSize	- размер массива
//Выход:
// - число байт
int  MakeHex(LPCTSTR sHex, PBYTE pBuf, int cbSize)
{
	CString sTmp = sHex, s;
	TCHAR *sStop;
	int j = 0, i = 0, k = 0;
	while( k < cbSize && ((j=sTmp.Find(_T(' '),i)) != -1 || (j=sTmp.Find(_T('\0'),i)) != -1) )
	{
		s = sTmp.Mid(i,j-i);
		pBuf[k++] = (BYTE)_tcstol(s,&sStop,16);
		i = j+1;
	}
	return k;
}

// Представить заданный байтовый массив в виде строки символов HEX формата
// LPTSTR pBufIn		- указатель на данные	
// int szIn				- длина данных(в байтах)
// LPTSTR pBufOut		- буфер для входной строки
// int szOut			- размер буфера pBufOut(в байтах)
//08.04.2011 Изменил
/*
void MakeHexStr(BYTE * pBufIn,int szIn,LPTSTR pBufOut, int szOut)
{
	if( !pBufIn || !pBufOut )
		return;

	memset (pBufOut,0,szOut);
	TCHAR ar[16];
	int Len = 0;
	for ( int i=0; i<szIn; i++ )
	{
		swprintf ( ar, _T("%.2X "), pBufIn[i] );
		wcscat (pBufOut,ar);
		Len += 4 * sizeof(TCHAR);
		if (Len >= (int) (szOut - (4 * sizeof(TCHAR))))
			break;
	}
}
*/
// Представить заданный байтовый массив в виде строки символов HEX формата
// LPTSTR pBufIn		- указатель на данные	
// int szIn				- длина данных
// LPTSTR pBufOut		- буфер для входной строки
// int szOut			- размер буфера pBufOut
void MakeHexStr(BYTE * pBufIn,int szIn,LPTSTR pBufOut, int szOut)
{
	if( !pBufIn || !pBufOut )
		return;

	memset (pBufOut,0,szOut);
	TCHAR ar[16];
	int Len = 0;
	for ( int i=0; i<szIn; i++ )
	{
		if (Len > (int) (szOut - (4 * sizeof(TCHAR))))
		{
			//08.04.2011 Незя,т.к. может быть меньше 4-х символов свободно в массиве
			//_tcscat (pBufOut,_T("..."));
			break;
		}
		_stprintf ( ar, _T("%.2X "), pBufIn[i] );
		_tcscat (pBufOut,ar);
		Len += 3 * sizeof(TCHAR);
	}
}


//Редактор ресурсов не позволяет уменьшать размеры боксов меньше заданных по-умолчанию.
//Поэтому, приходится уменьшать их программно.
void ResizeBoxStatus(CWnd *pWndOwner, UINT uID)
{
	if( NULL == pWndOwner )
		return;

	CWnd *pWnd = pWndOwner->GetDlgItem(uID);
	if (pWnd)
	{
		pWnd->SetWindowText(_T(""));
		pWnd->SetWindowPos(NULL,0,0,8,8,SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}


/////////////////////////////////////////////////////////////////////////
//Ф-я контролирует истечение заданного интервала Timeout
//относительно начального времени tBeg
/////////////////////////////////////////////////////////////////////////
//Вход:
// -  tBeg		- время начала контроля
// -  Timeout	- контролируемый интервал(mc) 
//Выход:
// - true/false  - интервал превышен/не превышен
/////////////////////////////////////////////////////////////////////////
bool IsTimeoutElapse(DWORD tBeg, DWORD Timeout)
{
	if( tBeg == 0 )
		return true;

	return (GetTickCount()-tBeg) > Timeout ? true : false;;
}



static unsigned short  crc16_table[256]=
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
  0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
  0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
  0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
  0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
  0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
  0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
  0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
  0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
  0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
  0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
  0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
  0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
  0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
  0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
  0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
  0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
  0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
  0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
  0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
  0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
  0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
  0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

// Расчет CRC16 табличным способом Диалог-Транс
WORD GetCrcRPC( WORD wCrc, BYTE* pBuf, int nSize)
{
	WORD crc = wCrc;
	WORD c1,c2;
	while (nSize--) 
	{			
		c1 = (crc >> 8);
		c2 = (WORD)((crc << 8)| *pBuf++); 
		crc = c2 ^ /*crc_ccitt_tab*/crc16_table[ c1 ];
	}
	crc = ~crc;
	BYTE lb, hb;
	lb = LOBYTE(crc); hb = HIBYTE(crc);
	return MAKEWORD(hb, lb);
}

/////////////////////////////////////////////////////////////////////////////
//---------------------- ЭЦ-МПК --------------------------------------------
//
unsigned int setunUpdCrc(unsigned char cp, unsigned int crc) // прямая CRC16
{
	unsigned int updcrc16;
 
	updcrc16 = crc16_table[(unsigned char)(crc >> 8)] ^ (crc << 8) ^ cp;
	return updcrc16;
}


//---------------------------------------------------------------------------
WORD setunFormCadr (unsigned char *cadr, unsigned int length) // формирование 
{
	// cadr - указатель на данные
	// length = длина блока данных
	WORD crc16=0;
	while(length--) 
		crc16 = setunUpdCrc(*(cadr++), crc16);
	crc16 = setunUpdCrc(0, crc16);
	crc16 = setunUpdCrc(0, crc16);
	*(cadr++)=(unsigned char)(crc16 >> 8); // старший байт
	*cadr=(unsigned char)crc16;            // младший байт
	return crc16;
}


//---------------------------------------------------------------------------
WORD setunReceiveCadr (unsigned char * cadr, unsigned int length) // разбор
{
	// cadr - указатель на данные
	// length = длина блока данных + 2 байта CRC16
	WORD crc16=0;
	while(length--) 
		crc16 = setunUpdCrc(*(cadr++), crc16);
	return crc16;							// 0 - ОК, не 0 - ошибка
}
//
/////////////////////////////////////////////////////////////////////////////


//Поместить ТС в pData
//Вход:
// - pData	- буфер для данных
// - cbSize - длина буфера в байтах
//Выход: длина длока в байтах
WORD MakeOutgoingTs(PBYTE pData, int cbSize)
{
	if( NULL == pData || cbSize == 0 )
		return 0;

	WORD Len = 0;

	int MaxMts = KP.GetNumMvv()*MAX_MTS_IN_MVV;
	UINT64 i64Real = KP.GetBitRealMTS();															//взять бит массив реальных МТС
	UINT64 i64 = 1;
	for( int j=0,m=0; m<MaxMts; m++,i64=i64<<1 )												//модуль
	{
		if( i64Real&i64 )																				//если модуль есть - выводим
		{
			for( int i=0; i<MTS_ROW_CNT; i++)
			{
				pData[j++] = TsStatus [m*MTS_ROW_CNT+i];
				if( j == cbSize )
					break;
			}
		}
		if( j == cbSize )
			break;
	}

	return (WORD)j;
}


/////////////////////////////////////////////////////////////////
//Поместить в пакет блок данных самодиагностики.
/////////////////////////////////////////////////////////////////
//Вход:		pBuf   - буфер собственно данных
//Выход:	длина блока данных в байтах
/////////////////////////////////////////////////////////////////
// 2008.02.12. BEG -------------------------------------------------------------------------
//					По просьбе разработчиков АДК СЦБ внесены изменения б блок диагностики:
//					1. Поля "5,6,7", и "9,10,11" объединены и идут сплошным массивом 6 байт
//					2. В шести байтах кодируются отказы модулей ТС в упакованном виде
//					   в соответствии с порядком упаковки информации по модулям в блоке ТС	
//					3. Поле "8" переместилось в позицию 11
//					Точно такой же блок передается в АПКДК
// 2008.02.12. END -------------------------------------------------------------------------
/*?*/
//27.07.2015 Вариант без учета активного АРМ.Инфа формирется всегда как для АРМ ДНЦ
//Ниже вариант с учетом активного АРМ
/*
WORD MakeOutgoingDiag(PBYTE pBuf)
{
    if( NULL == pBuf )
		return 0;

	PBYTE pData = pBuf;
	BYTE Sts = 0;
	WORD Len = 0;

// 0 байт - байт состояния связи
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------BEG
	if( pPassivMdm )
	{
		if (pPassivMdm->IsConnected())														// Бит
			Sts |= MSK_COM3_CONNECT;														// 0 - "1"- наличие соединения на СОМ3
	    if (!pPassivMdm->IsOpen())		//if(MdmInitCom3==-1)
	        Sts |= MSK_COM3_INIER;															// 2 - "1"- ош.инициализации модема на СОМ3
	}
	if( pActivMdm )
	{
		if (pActivMdm->IsConnected())
			Sts |= MSK_COM4_CONNECT;														// 1 - "1"- наличие соединения на СОМ4
		if (!pActivMdm->IsOpen())		//if(MdmInitCom4==-1)
			Sts |= MSK_COM4_INIER;															// 3 - "1"- ош.инициализации модема на СОМ4
	}
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------END

	if (KP.IsKpByPass())
		Sts |= MSK_BYPASS;																	// 4 - "1"- байпас КП
	if (KP.IsCmpTs())
        Sts |= MSK_CMP_TS;																	// 5 - "1"- сост. сигналов ТС своего и сосед. равны
    // 6 - "1"- признак упакованных данных(проставляется в ф-ии MakeTs())
	//17.08.2015 Изменил  с учетом ДСП.
	//Здесь логика такая.Даже для пассивного АРМ нужно дать готовность активного СПОК.
	//Поэтому всегда определяем активный АРМ и для него определяем готовность СПОК
	int aArm = GetActiveArm();
	if( gUpok[aArm] == TYPE_SPOK )
	{
		if( (pRSSpok[0] && pRSSpok[0]->IsConnected()) || 
			(pRSSpok[1] && pRSSpok[1]->IsConnected()) )
		{
			Sts |= MSK_SPOK_OK;																	// 7 - "1"-ОМУЛ ОК
		}
	}
	else
	if( gUpok[aArm] == TYPE_UPOK )
	{
		if( pUpok && pUpok->IsOK() )
		{
			Sts |= MSK_SPOK_OK;																	// 7 - "1"-ОМУЛ ОК
		}
	}
	//03.10.2012 Изменил с учетом 2-х СПОК
	//if( (pRSSpok[0] && pRSSpok[0]->IsConnected()) || 
	//	(pRSSpok[1] && pRSSpok[1]->IsConnected()) || 
	//06.11.2009 Добавил готовность УПОК
	//if( (pRSSpok && pRSSpok->IsConnected()) || 
	//	(gUpok && pUpok && pUpok->IsOK()) )
	//{
	//	Sts |= MSK_SPOK_OK;																	// 7 - "1"-ОМУЛ ОК
	//}

    pData[SYS_OFS_LINK_STS    ] = Sts;
    Len++;

//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------BEG
	pData[SYS_OFS_COM3_SPEED  ] = 0;
	pData[SYS_OFS_COM3_SPEED+1] = 0;
	if( pPassivMdm )
	{
// 1 - скорость/1200   на СОМ3
		pData[SYS_OFS_COM3_SPEED  ] = (BYTE)(_ttoi(pPassivMdm->GetSpeed())/1200);				// скорость на COM3
// 2 - число RECONNECT на COM3.
		pData[SYS_OFS_COM3_SPEED+1] = pPassivMdm->GetReconnectCnt();							// число RECONNECT на COM3
	}
    pData[SYS_OFS_COM4_SPEED  ] = 0;
	pData[SYS_OFS_COM4_SPEED+1] = 0;
	if( pActivMdm )
	{
// 3 - скорость/1200   на СОМ4
		pData[SYS_OFS_COM4_SPEED  ] = (BYTE)(_ttoi(pActivMdm->GetSpeed())/1200);				// скорость/1200   на СОМ4
// 4 - число RECONNECT на СОМ4
		pData[SYS_OFS_COM4_SPEED+1] = pActivMdm->GetReconnectCnt();							// число RECONNECT на СОМ4
	}
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------END
    Len += 4;


// 2008.02.12. BEG -------------------------------------------------------------------------
// Комментируем след.блок кода
// 5,6,7 - состояние ТУ/ТС МВВ1(0 бит - 1 модуль, 23 бит - 24 модуль)
//	CKPFastwel *pKp = (CKPFastwel*)&KP;
//	pKp->GetErrorIO( &pData[Len],3,0 );	 // диагностика: ош. МВВ 1 - 3 байта
//    Len += 3;

// Блок сисинфо для АДКСЦБ и АПКДК будет отличаться от оригинального
// 5,6,7,8,9,10 - шесть байт с отказами МТС упакованными без пропусков и модулей ТУ 
// (точно так, как они передаются в блоке ТС)
	memset (&pData[Len], 0, 6);
	UINT64 i64Real = KP.GetBitRealMTS();
	UINT64 i64 = 1;
	int indx = 0;
	//13.05.2008 С версии 1.0.7.10. исправил
	//for ( int i=0; i<48; i++ )								// строго до 48, так как у нас всего 6 байт
	for ( int i=0; i<48; i++,i64=i64<<1 )								// строго до 48, так как у нас всего 6 байт
	{
		if ( i64Real & i64 )
		{
			if (KP.GetModulStatus(i))						// вызовет вирт.функцию ?
			{
				//pData[Len + indx/8] |= (indx % 8) << 1;	//13.05.2008 С версии 1.0.7.10. исправлено
				pData[Len + indx/8] |= (1 << (indx % 8));
				indx++;
			}
		}
	}
	Len += 6;										// длина = 11
// 2008.02.12. END -------------------------------------------------------------------------

	
// 8 - состояние регистров МКУ
    Sts = 0;
    if (!KP.IsMain())					//Бит
        Sts |= MSK_MAINRSRV;			// 0 - "0"-осн, "1" -резерв
    // я на линии
	if (KP.IsLineOnOne())				
        Sts |= MSK_ONE_ONLINE;			// 1 - "1" - акт. блок подкл. к линии
    // я подключен к объекту
	if (KP.IsControlOne())				
        Sts |= MSK_ONE_CONTROL;			// 2 - "1" - акт. блок подкл. к объекту
    // сосед на линии
	if (KP.IsLineOnAnother ())			
        Sts |= MSK_ANOTHER_ONLINE;		// 3 - "1" - неакт. блок подкл. к линии
    // сосед подключен к объекту
	if (KP.IsControlAnother ())			
        Sts |= MSK_ANOTHER_CONTROL;		// 4 - "1" - неакт. блок подкл. к объекту

	//time_t tm = CTime::GetCurrentTime().GetTime();

	//Если пакет не получен втечении 10сек, считаем ошибкой
	//10.10.2008 А.А. Для ЭЦЕМ используется тот же флаг, что и для МПЦ
	//01.03.2011 Удалил pRSMpc, актуален только указатель pRSAcem
	if( //(pRSMpc && pRSMpc->IsOK()) || 
		//10.10.2008 А.А. Для ЭЦЕМ используется тот же флаг, что и для МПЦ
		((pRSAcem[0] && pRSAcem[0]->IsOK()) || (pRSAcem[1] && pRSAcem[1]->IsOK())) ||
		//09.04.2010 А.А. Для ЭЦ-МПК используется тот же флаг, что и для МПЦ
		((pRSAcmpk[0] && pRSAcmpk[0]->IsOK()) || (pRSAcmpk[1] && pRSAcmpk[1]->IsOK())) )
	{
		Sts |= MSK_MPC_OK;				// 5 - "1" - EBILOCK=OK, "0"-ошибка или отсутствие или
										//     "1" - ЭЦЕМ   =OK, "0"-ошибка или отсутствие
										//     "1" - ЭЦ-МПК =OK, "0"-ошибка или отсутствие
	}

	//05.09.2007.Если пакет не получен втечении 5сек, считаем ошибкой)
	if( pRSRpc && pRSRpc->IsOK() ) 
		Sts |= MSK_RPC_OK;				// 6 - "1" - РПЦДИАЛОГ=OK, "0"-ошибка или отсутствие

	//Признак - режим тестирования
	//12.05.2008 Ализируется состояние теста StsTestMts,StsTestMtu
	//if( KP.IsTestMts() || KP.IsTestMtu() )
	//12.05.2008 Ализируется номер тестируемого модуля wTestingMTS,wTestingMTU
	 if( KP.IsTestingMTS() || KP.IsTestingMTU() )
		Sts |= MSK_TEST_MODE;			// 7 - "1" - в режиме тестирования МТУ / МТС
// 2008.02.12. BEG -------------------------------------------------------------------------
//    pData[SYS_OFS_MKU_STS] = Sts;
	pData[11] = Sts;					// 8-й байт стал 11-м
// 2008.02.12. END -------------------------------------------------------------------------

    Len++;								// Len = 12

// 2008.02.12. BEG -------------------------------------------------------------------------
// Блок ТУ упразднен. См.выше
// 9,10,11 - состояние ТУ/ТС МВВ2(0 бит - 25 модуль, 23 бит - 48 модуль)
//   	if( KP.GetNumMvv() >= 2 )
//	{
//		pKp->GetErrorIO( &pData[Len],3,1 );	 // диагностика: ош. МВВ 2 - 3 байта
//	}
//	Len += 3;
// 2008.02.12. END -------------------------------------------------------------------------


//12 - МВВ1,2: АТУ,ОШ.ключа,Ош.выхода; АДКСЦБ,АПКДК
	Sts = 0;
	//22.10.2008 А.А. Добавили ЭЦЕМ
	//if( !pRSMpc && !pRSRpc && KP.GetRealMTU() )
	//23.04.2010 А.А. Добавили ЭЦМПК
	//01.03.2011 Удалил pRSMpc, актуален только указатель pRSAcem
	if( //!pRSMpc      && 
		!pRSRpc			&& 
		!pRSAcem[0]  && !pRSAcem[1]		&& 
		!pRSAcmpk[0] && !pRSAcmpk[1]	&& KP.GetRealMTU() )
	{
		//27.09.2011 Добавил контроль состояния готовности ключа и подключения к объекту
		//24.11.2009 только для существующих МВВ.
   		if( KP.IsMvvSet(0) )
		{
			if( KP.IsATU(0) )					//Бит
				Sts |= MSK_MVV1_ATU;			// 0 - "1"-АТУ МВВ1
			//if (KP.GetStatusTuKeyReady(0) == 1 || KP.GetStatusTuKeyReady(0) == 2 )
			if (KP.GetStatusTuKeyReady(0) == 1 || KP.GetStatusTuKeyReady(0) == 2 || gTuKeyErr[0] )
				Sts |= MSK_MVV1_KEY_ER;			// 1 - "1"-Ош.готовности ключа МВВ1
			//if (KP.GetStatusTuOutReady(0) == 1 || KP.GetStatusTuOutReady(0) == 2)
			if (KP.GetStatusTuOutReady(0) == 1 || KP.GetStatusTuOutReady(0) == 2 || gTuOutErr[0] )
				Sts |= MSK_MVV1_OUT_ER;			// 2 - "1"-Ош.готовности выхода МВВ1
		}
   		if( KP.IsMvvSet(1) )
		{
			if( KP.IsATU(1) )
				Sts |= MSK_MVV2_ATU;			// 3 - "1"-АТУ МВВ2
			//if (KP.GetStatusTuKeyReady(1) == 1 || KP.GetStatusTuKeyReady(1) == 2 )
			if (KP.GetStatusTuKeyReady(1) == 1 || KP.GetStatusTuKeyReady(1) == 2 || gTuKeyErr[1] )
				Sts |= MSK_MVV2_KEY_ER;			// 4 - "1"-Ош.готовности ключа МВВ2
			//if (KP.GetStatusTuOutReady(1) == 1 || KP.GetStatusTuOutReady(1) == 2)
			if (KP.GetStatusTuOutReady(1) == 1 || KP.GetStatusTuOutReady(1) == 2 || gTuOutErr[1] )
				Sts |= MSK_MVV2_OUT_ER;			// 5 - "1"-Ош.готовности выхода МВВ2
		}
	}
	//Если пакет не получен в течении 5сек, считаем ошибкой
	if( pRSStdm && pRSStdm->IsOK() )
		Sts |= MSK_STDM_OK;				// 6 - "1" - АДКСЦБ=OK, "0"-ошибка или отсутствие
	
	//???(05.09.2007.Временно. Если пакет не получен в течении 5сек, считаем ошибкой)
	//if( pRSApkdk && pRSApkdk->IsOK() )
	//02.08.2008 Бит работоспособности АПКДК или АБТЦМ
	if( (pRSApkdk    && pRSApkdk   ->IsOK())  || 
		(pRSKab      && pRSKab     ->IsOK())  ||	//Если пакет не получен в течении 5сек
		(pRSAbtcm[0] && pRSAbtcm[0]->IsOK_()) ||	//Если пакет не получен в течении 15сек
		(pRSAbtcm[1] && pRSAbtcm[1]->IsOK_()) ||	//
		(pRSAbtcm[2] && pRSAbtcm[2]->IsOK_()) )		//
	{
		Sts |= MSK_APKDK_OK;			// 7 - "1" - АПКДК=OK, "0"-ошибка или отсутствие
										//или	   - АБТЦМ=OK, "0"-ошибка или отсутствие
										//или	   - КЭБ  =OK, "0"-ошибка или отсутствие			
	}
    pData[SYS_OFS_MVV_DIAG] = Sts;
    Len++;

	//07.08.2010 А.А. Изменил.
	//13 байт - номер версии программы
	pData[Len] = gVer;
	Len++;
	
	//07.08.2010 А.А. 14 байт. 
	Sts = 0;
	//Бит D7 - признак направления опроса:
	// если последний опрос по СОМ3, D7 = "0" - основной канал
	// если последний опрос по СОМ4, D7 = "1" - обводной канал
	time_t tPassiv = 0, tActiv = 0;
	if( pPassivMdm )
		tPassiv = pPassivMdm->GetMdmLastTimeRecv();
	if( pActivMdm )
		tActiv = pActivMdm->GetMdmLastTimeRecv();
	if( tPassiv < tActiv)
		Sts |= MSK_DIR_POLL;						// опрос по обводному каналу

	//17.08.2015 Изменил
	//Но ВОПРОС!Здесь надо проверить указанный АРМ или активный?
	//Выбираю активный.ПРОВЕРИТЬ
	//if( gUpok[iArm] == TYPE_UPOK && pUpok )
	if( gUpok[aArm] == TYPE_UPOK && pUpok )
	//23.03.2012 Готовность УПОК(IsOk()) перенес выше(бит СПОК)
	//if( gUpok && pUpok )
	//20.05.2011 Изменил, добавил проверку схемы аппаратной готовности ОТУ
	//if( gUpok && pUpok && pUpok->IsOtuOn() )
	//18.11.2009 Бит D6 - состояние выдачи ОТУ
	// D6 = "0" - не в режиме выдачи ОТУ или были ошибки в процессе выдачи
	//      "1" - в режиме выдачи ОТУ, процесс выдачи в норме
	//if( gUpok && pUpok && pUpok->IsOK() && pUpok->IsOtuOn() )
	{
		if( CUpok::Cmd && pUpok->IsOtuShemaOk() )
			Sts |= MSK_OTU_OK;						//"1" - сотояние выдачи ОТУ в норме
		//23.05.2011 Вариант для отдельно выделенного бита
		//if( pUpok->IsOtuShemaOk() )
		//	Sts |= MSK_OTU_SHEMA_OK;				//"1" - сотояние работоспособности схемы готовности ОТУ
	}

	//07.08.2010 Бит D5 - статус сторожевого таймера
	if( gWatchDogEnable )
		Sts |= MSK_WDOG;							//"1" - сторожевой таймер установлен 

	//07.08.2010 Бит D4 - доступное ОЗУ меньше допустимого(1Мб)
	MEMORYSTATUS MemBuffer;
	GlobalMemoryStatus( &MemBuffer );
	if( MemBuffer.dwAvailPhys < 0x0100000 )			// если < 1Мб
		Sts |= MSK_MEMOFF;							//"1" - ОЗУ меньше допуcтимого				

	//07.08.2010 Бит D3 - признак ретрансляции
	if( gRetrans )
		Sts |= MSK_RETRANS;							//"1" - ретрансляция включена

	//09.11.2011 Бит D2 - признак подключения к отладочному порту
	if( CRsDebug::bDbgPort )
		Sts |= MSK_DBGPORT;							//"1" - подключение

	//17.11.2011 Бит D1 - признак вкл/откл режима отладки ОТУ
	if( CRsDebug::bDbgTu )
		Sts |= MSK_DBGOTU;							//"1" - подключение

	//23.04.2012 Бит D0 - признак вкл режима СУ ДСП
	if( IsSuDsp() )
		Sts |= MSK_SU_DSP;							//"1" - подключение

	pData[Len] = Sts;
	Len++;

	// 15-29 - блок SYSINFO соседнего блока

	return Len;
}
*/

//27.07.2015 Вариант с учетом активного АРМ - формируем для активного АРМ
WORD MakeOutgoingDiag(PBYTE pBuf)
{
    if( NULL == pBuf )
		return 0;

	PBYTE pData = pBuf;
	BYTE Sts = 0;
	WORD Len = 0;

	int aArm = GetActiveArm(false);				// false - без учета DBG порта
	if( aArm == ARM_DSP )
	//if( IsSuDsp() )
	{
		pPasMdm[ARM_DBG] = pPassivMdmDsp;
		pActMdm[ARM_DBG] = pActivMdmDsp;
	}
	else
	{
		pPasMdm[ARM_DBG] = pPassivMdm;
		pActMdm[ARM_DBG] = pActivMdm;
	}

// 0 байт - байт состояния связи
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------BEG
	if( pPasMdm[aArm] )
	{
		if (pPasMdm[aArm]->IsConnected())														// Бит
			Sts |= MSK_COM3_CONNECT;														// 0 - "1"- наличие соединения на СОМ3
	    if (!pPasMdm[aArm]->IsOpen())		//if(MdmInitCom3==-1)
	        Sts |= MSK_COM3_INIER;															// 2 - "1"- ош.инициализации модема на СОМ3
	}
	if( pActMdm[aArm] )
	{
		if (pActMdm[aArm]->IsConnected())
			Sts |= MSK_COM4_CONNECT;														// 1 - "1"- наличие соединения на СОМ4
		if (!pActMdm[aArm]->IsOpen())		//if(MdmInitCom4==-1)
			Sts |= MSK_COM4_INIER;															// 3 - "1"- ош.инициализации модема на СОМ4
	}
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------END

	if (KP.IsKpByPass())
		Sts |= MSK_BYPASS;																	// 4 - "1"- байпас КП
	if (KP.IsCmpTs())
        Sts |= MSK_CMP_TS;																	// 5 - "1"- сост. сигналов ТС своего и сосед. равны
    // 6 - "1"- признак упакованных данных(проставляется в ф-ии MakeTs())
/*?*///02.10.2015 Изменил  с учетом ДСП.
	//Логика такая:
	// - даем готовность на активном
	if( gUpok[aArm]==TYPE_SPOK )
	{
		if( (pRSSpok[0] && pRSSpok[0]->IsConnected()) || 
			(pRSSpok[1] && pRSSpok[1]->IsConnected()) )
		{
			Sts |= MSK_SPOK_OK;												// 7 - "1"-ОМУЛ ОК
		}
	}
	else
	if( gUpok[aArm]==TYPE_UPOK )
	{
		if( pUpok && pUpok->IsOK() )
		{
			Sts |= MSK_SPOK_OK;												// 7 - "1"-ОМУЛ ОК
		}
	}
	//03.10.2012 Изменил с учетом 2-х СПОК
	//if( (pRSSpok[0] && pRSSpok[0]->IsConnected()) || 
	//	(pRSSpok[1] && pRSSpok[1]->IsConnected()) || 
	//06.11.2009 Добавил готовность УПОК
	//if( (pRSSpok && pRSSpok->IsConnected()) || 
	//	(gUpok && pUpok && pUpok->IsOK()) )
	//{
	//	Sts |= MSK_SPOK_OK;																	// 7 - "1"-ОМУЛ ОК
	//}

    pData[SYS_OFS_LINK_STS    ] = Sts;
    Len++;

//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------BEG
	pData[SYS_OFS_COM3_SPEED  ] = 0;
	pData[SYS_OFS_COM3_SPEED+1] = 0;
	if( pPasMdm[aArm] )
	{
// 1 - скорость/1200   на СОМ3
		pData[SYS_OFS_COM3_SPEED  ] = (BYTE)(_ttoi(pPasMdm[aArm]->GetSpeed())/1200);				// скорость на COM3
// 2 - число RECONNECT на COM3.
		pData[SYS_OFS_COM3_SPEED+1] = pPasMdm[aArm]->GetReconnectCnt();							// число RECONNECT на COM3
	}
    pData[SYS_OFS_COM4_SPEED  ] = 0;
	pData[SYS_OFS_COM4_SPEED+1] = 0;
	if( pActMdm[aArm] )
	{
// 3 - скорость/1200   на СОМ4
		pData[SYS_OFS_COM4_SPEED  ] = (BYTE)(_ttoi(pActMdm[aArm]->GetSpeed())/1200);				// скорость/1200   на СОМ4
// 4 - число RECONNECT на СОМ4
		pData[SYS_OFS_COM4_SPEED+1] = pActMdm[aArm]->GetReconnectCnt();							// число RECONNECT на СОМ4
	}
//16.02.2009 А.А. Изменил. Вставил контроль указателей pPassivMdm,pActivMdm ---------END
    Len += 4;


// 2008.02.12. BEG -------------------------------------------------------------------------
// Комментируем след.блок кода
// 5,6,7 - состояние ТУ/ТС МВВ1(0 бит - 1 модуль, 23 бит - 24 модуль)
//	CKPFastwel *pKp = (CKPFastwel*)&KP;
//	pKp->GetErrorIO( &pData[Len],3,0 );	 // диагностика: ош. МВВ 1 - 3 байта
//    Len += 3;

// Блок сисинфо для АДКСЦБ и АПКДК будет отличаться от оригинального
// 5,6,7,8,9,10 - шесть байт с отказами МТС упакованными без пропусков и модулей ТУ 
// (точно так, как они передаются в блоке ТС)
	memset (&pData[Len], 0, 6);
	UINT64 i64Real = KP.GetBitRealMTS();
	UINT64 i64 = 1;
	int indx = 0;
	//13.05.2008 С версии 1.0.7.10. исправил
	//for ( int i=0; i<48; i++ )								// строго до 48, так как у нас всего 6 байт
	for ( int i=0; i<48; i++,i64=i64<<1 )								// строго до 48, так как у нас всего 6 байт
	{
		if ( i64Real & i64 )
		{
			if (KP.GetModulStatus(i))						// вызовет вирт.функцию ?
			{
				//pData[Len + indx/8] |= (indx % 8) << 1;	//13.05.2008 С версии 1.0.7.10. исправлено
				pData[Len + indx/8] |= (1 << (indx % 8));
				indx++;
			}
		}
	}
	Len += 6;										// длина = 11
// 2008.02.12. END -------------------------------------------------------------------------

	
// 8 - состояние регистров МКУ
    Sts = 0;
    if (!KP.IsMain())					//Бит
        Sts |= MSK_MAINRSRV;			// 0 - "0"-осн, "1" -резерв
    // я на линии
	if (KP.IsLineOnOne())				
        Sts |= MSK_ONE_ONLINE;			// 1 - "1" - акт. блок подкл. к линии
    // я подключен к объекту
	if (KP.IsControlOne())				
        Sts |= MSK_ONE_CONTROL;			// 2 - "1" - акт. блок подкл. к объекту
    // сосед на линии
	if (KP.IsLineOnAnother ())			
        Sts |= MSK_ANOTHER_ONLINE;		// 3 - "1" - неакт. блок подкл. к линии
    // сосед подключен к объекту
	if (KP.IsControlAnother ())			
        Sts |= MSK_ANOTHER_CONTROL;		// 4 - "1" - неакт. блок подкл. к объекту

	//time_t tm = CTime::GetCurrentTime().GetTime();

	//Если пакет не получен втечении 10сек, считаем ошибкой
	//10.10.2008 А.А. Для ЭЦЕМ используется тот же флаг, что и для МПЦ
	//01.03.2011 Удалил pRSMpc, актуален только указатель pRSAcem
	if( //(pRSMpc && pRSMpc->IsOK()) || 
		//10.10.2008 А.А. Для ЭЦЕМ используется тот же флаг, что и для МПЦ
		((pRSAcem[0] && pRSAcem[0]->IsOK()) || (pRSAcem[1] && pRSAcem[1]->IsOK())) ||
		//09.04.2010 А.А. Для ЭЦ-МПК используется тот же флаг, что и для МПЦ
		((pRSAcmpk[0] && pRSAcmpk[0]->IsOK()) || (pRSAcmpk[1] && pRSAcmpk[1]->IsOK())) )
	{
		Sts |= MSK_MPC_OK;				// 5 - "1" - EBILOCK=OK, "0"-ошибка или отсутствие или
										//     "1" - ЭЦЕМ   =OK, "0"-ошибка или отсутствие
										//     "1" - ЭЦ-МПК =OK, "0"-ошибка или отсутствие
	}

	//05.09.2007.Если пакет не получен втечении 5сек, считаем ошибкой)
	if( pRSRpc && pRSRpc->IsOK() ) 
		Sts |= MSK_RPC_OK;				// 6 - "1" - РПЦДИАЛОГ=OK, "0"-ошибка или отсутствие

	//Признак - режим тестирования
	//12.05.2008 Ализируется состояние теста StsTestMts,StsTestMtu
	//if( KP.IsTestMts() || KP.IsTestMtu() )
	//12.05.2008 Ализируется номер тестируемого модуля wTestingMTS,wTestingMTU
	 if( KP.IsTestingMTS() || KP.IsTestingMTU() )
		Sts |= MSK_TEST_MODE;			// 7 - "1" - в режиме тестирования МТУ / МТС
// 2008.02.12. BEG -------------------------------------------------------------------------
//    pData[SYS_OFS_MKU_STS] = Sts;
	pData[11] = Sts;					// 8-й байт стал 11-м
// 2008.02.12. END -------------------------------------------------------------------------

    Len++;								// Len = 12

// 2008.02.12. BEG -------------------------------------------------------------------------
// Блок ТУ упразднен. См.выше
// 9,10,11 - состояние ТУ/ТС МВВ2(0 бит - 25 модуль, 23 бит - 48 модуль)
//   	if( KP.GetNumMvv() >= 2 )
//	{
//		pKp->GetErrorIO( &pData[Len],3,1 );	 // диагностика: ош. МВВ 2 - 3 байта
//	}
//	Len += 3;
// 2008.02.12. END -------------------------------------------------------------------------


//12 - МВВ1,2: АТУ,ОШ.ключа,Ош.выхода; АДКСЦБ,АПКДК
	Sts = 0;
	//22.10.2008 А.А. Добавили ЭЦЕМ
	//if( !pRSMpc && !pRSRpc && KP.GetRealMTU() )
	//23.04.2010 А.А. Добавили ЭЦМПК
	//01.03.2011 Удалил pRSMpc, актуален только указатель pRSAcem
	if( //!pRSMpc      && 
		!pRSRpc			&& 
		!pRSAcem[0]  && !pRSAcem[1]		&& 
		!pRSAcmpk[0] && !pRSAcmpk[1]	&& KP.GetRealMTU() )
	{
		//27.09.2011 Добавил контроль состояния готовности ключа и подключения к объекту
		//24.11.2009 только для существующих МВВ.
   		if( KP.IsMvvSet(0) )
		{
			if( KP.IsATU(0) )					//Бит
				Sts |= MSK_MVV1_ATU;			// 0 - "1"-АТУ МВВ1
			//if (KP.GetStatusTuKeyReady(0) == 1 || KP.GetStatusTuKeyReady(0) == 2 )
			if (KP.GetStatusTuKeyReady(0) == 1 || KP.GetStatusTuKeyReady(0) == 2 || gTuKeyErr[0] )
				Sts |= MSK_MVV1_KEY_ER;			// 1 - "1"-Ош.готовности ключа МВВ1
			//if (KP.GetStatusTuOutReady(0) == 1 || KP.GetStatusTuOutReady(0) == 2)
			if (KP.GetStatusTuOutReady(0) == 1 || KP.GetStatusTuOutReady(0) == 2 || gTuOutErr[0] )
				Sts |= MSK_MVV1_OUT_ER;			// 2 - "1"-Ош.готовности выхода МВВ1
		}
   		if( KP.IsMvvSet(1) )
		{
			if( KP.IsATU(1) )
				Sts |= MSK_MVV2_ATU;			// 3 - "1"-АТУ МВВ2
			//if (KP.GetStatusTuKeyReady(1) == 1 || KP.GetStatusTuKeyReady(1) == 2 )
			if (KP.GetStatusTuKeyReady(1) == 1 || KP.GetStatusTuKeyReady(1) == 2 || gTuKeyErr[1] )
				Sts |= MSK_MVV2_KEY_ER;			// 4 - "1"-Ош.готовности ключа МВВ2
			//if (KP.GetStatusTuOutReady(1) == 1 || KP.GetStatusTuOutReady(1) == 2)
			if (KP.GetStatusTuOutReady(1) == 1 || KP.GetStatusTuOutReady(1) == 2 || gTuOutErr[1] )
				Sts |= MSK_MVV2_OUT_ER;			// 5 - "1"-Ош.готовности выхода МВВ2
		}
	}
	//Если пакет не получен в течении 5сек, считаем ошибкой
	if( pRSStdm && pRSStdm->IsOK() )
		Sts |= MSK_STDM_OK;				// 6 - "1" - АДКСЦБ=OK, "0"-ошибка или отсутствие
	
	//???(05.09.2007.Временно. Если пакет не получен в течении 5сек, считаем ошибкой)
	//if( pRSApkdk && pRSApkdk->IsOK() )
	//02.08.2008 Бит работоспособности АПКДК или АБТЦМ
	if( (pRSApkdk    && pRSApkdk   ->IsOK())  || 
		(pRSKab      && pRSKab     ->IsOK())  ||	//Если пакет не получен в течении 5сек
		(pRSAbtcm[0] && pRSAbtcm[0]->IsOK_()) ||	//Если пакет не получен в течении 15сек
		(pRSAbtcm[1] && pRSAbtcm[1]->IsOK_()) ||	//
		(pRSAbtcm[2] && pRSAbtcm[2]->IsOK_()) )		//
	{
		Sts |= MSK_APKDK_OK;			// 7 - "1" - АПКДК=OK, "0"-ошибка или отсутствие
										//или	   - АБТЦМ=OK, "0"-ошибка или отсутствие
										//или	   - КЭБ  =OK, "0"-ошибка или отсутствие			
	}
    pData[SYS_OFS_MVV_DIAG] = Sts;
    Len++;

	//07.08.2010 А.А. Изменил.
	//13 байт - номер версии программы
	pData[Len] = gVer;
	Len++;
	
	//07.08.2010 А.А. 14 байт. 
	Sts = 0;
	//Бит D7 - признак направления опроса:
	// если последний опрос по СОМ3, D7 = "0" - основной канал
	// если последний опрос по СОМ4, D7 = "1" - обводной канал
	time_t tPassiv = 0, tActiv = 0;
	if( pPasMdm[aArm] )
		tPassiv = pPasMdm[aArm]->GetMdmLastTimeRecv();
	if( pActMdm[aArm] )
		tActiv = pActMdm[aArm]->GetMdmLastTimeRecv();
	if( tPassiv < tActiv)
		Sts |= MSK_DIR_POLL;						// опрос по обводному каналу

/*?*///02.10.2015 Если хотя бы на одном есть УПОК - устанавливаем флаг
	if( GetArmUpok()!=-1 && pUpok )
	//23.03.2012 Готовность УПОК(IsOk()) перенес выше(бит СПОК)
	//if( gUpok && pUpok )
	//20.05.2011 Изменил, добавил проверку схемы аппаратной готовности ОТУ
	//if( gUpok && pUpok && pUpok->IsOtuOn() )
	//18.11.2009 Бит D6 - состояние выдачи ОТУ
	// D6 = "0" - не в режиме выдачи ОТУ или были ошибки в процессе выдачи
	//      "1" - в режиме выдачи ОТУ, процесс выдачи в норме
	//if( gUpok && pUpok && pUpok->IsOK() && pUpok->IsOtuOn() )
	{
		if( CUpok::Cmd && pUpok->IsOtuShemaOk() )
			Sts |= MSK_OTU_OK;						//"1" - сотояние выдачи ОТУ в норме
		//23.05.2011 Вариант для отдельно выделенного бита
		//if( pUpok->IsOtuShemaOk() )
		//	Sts |= MSK_OTU_SHEMA_OK;				//"1" - сотояние работоспособности схемы готовности ОТУ
	}

	//07.08.2010 Бит D5 - статус сторожевого таймера
	if( gWatchDogEnable )
		Sts |= MSK_WDOG;							//"1" - сторожевой таймер установлен 

	//07.08.2010 Бит D4 - доступное ОЗУ меньше допустимого(1Мб)
	MEMORYSTATUS MemBuffer;
	GlobalMemoryStatus( &MemBuffer );
	if( MemBuffer.dwAvailPhys < 0x0100000 )			// если < 1Мб
		Sts |= MSK_MEMOFF;							//"1" - ОЗУ меньше допуcтимого				

	//07.08.2010 Бит D3 - признак ретрансляции
	if( gRetrans[aArm] )
		Sts |= MSK_RETRANS;							//"1" - ретрансляция включена

	//09.11.2011 Бит D2 - признак подключения к отладочному порту
	if( CRsDebug::bDbgPort )
		Sts |= MSK_DBGPORT;							//"1" - подключение

	//17.11.2011 Бит D1 - признак вкл/откл режима отладки ОТУ
	if( CRsDebug::bDbgTu )
		Sts |= MSK_DBGOTU;							//"1" - подключение

  //28.07.2015 Изменил,т.к.добавили режим РСУ(то же, что и СУ)
	if( IsDsp() && (IsSu()||IsRsu()) )
	//23.04.2012 Бит D0 - признак вкл режима СУ ДСП
	//if( IsSuDsp() )
		Sts |= MSK_SU_DSP;							//"1" - подключение

	pData[Len] = Sts;
	Len++;

	// 15-29 - блок SYSINFO соседнего блока

	return Len;
}


//запомнить в реестре директивы
//Если dwDelay!=0 - сброс реестра на флеш(RegFlushKey()) с задержкой dwDelay
void SaveSysCmdToReg(DWORD dwDelay)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	//запоминаем список последних директив
	WriteRegBin (gRegNameKey,gRegNameLastCmd,GetPtrListSys(),GetSizeListSys());

	//15.08.2011 Восстановил
	//27.07.2011 Изменил, использую поток для записи
	if( dwDelay )
		FlushRegKey(gRegNameKey, dwDelay);
//	bFlushKP = true;
//	SetEvent(ghThreadFlushRegEvent);

	//28.07.2011 Поскольку сброс реестра на флэш выполняется из другого потока, 
	//надо дождаться,когда сброс исполнится
	//if( dwDelay )
	//	Sleep(dwDelay);

	SetCursor(hCurrentCursor);
}


///////////////////////////////////////////////////////////////////////////////////
// Функция лога 
int udpLogPrt = 7010;
bool  udpLogError = false;

// Представить заданный байтовый массив в виде строки символов HEX формата
// LPTSTR pBufIn		- указатель на данные	
// int szIn				- длина данных(в байтах)
// LPTSTR pBufOut		- буфер для входной строки
// int szOut			- размер буфера pBufOut(в байтах)
//11.04.2011 Изменил
/*
void MakeHexStrANSII(BYTE * pBufIn,int szIn,char * pBufOut, int szOut)
{
	if( !pBufIn || !pBufOut )
		return;

	memset (pBufOut,0,szOut);
	char ar[16];
	int Len = 0;
	for ( int i=0; i<szIn; i++ )
	{
		sprintf ( ar, "%.2X ", pBufIn[i] );
		strcat (pBufOut,ar);
		Len += 4 * sizeof(char);
		if (Len >= (int) (szOut - (4 * sizeof(char))))
			break;
	}
}
*/
void MakeHexStrANSII(BYTE * pBufIn,int szIn,char * pBufOut, int szOut)
{
	if( !pBufIn || !pBufOut )
		return;

	memset (pBufOut,0,szOut);
	char ar[16];
	int Len = 0;
	for ( int i=0; i<szIn; i++ )
	{
		if (Len > (int) (szOut - (4 * sizeof(char))))
			break;
		sprintf ( ar, "%.2X ", pBufIn[i] );
		strcat (pBufOut,ar);
		Len += 3 * sizeof(char);
	}
}


// Делегат, вызываемый CUdpClient. Ничего не делаем
void CALLBACK funcUdpLog (LPARAM p)
{
	CUdpClient * pClient = (CUdpClient * )p;
	DWORD mode = pClient->GetStatusNotify();

	switch (mode)
	{
		case CUdpClient::DELEGATE_RECEIVE:
			break;
		case CUdpClient::DELEGATE_ERROR:
			udpLogError = true;
			break;
	}
}


// вывод строки байт в кодировке 1251
bool udpLog (BYTE *ptr, int len)
{
	//CUdpClient sender(funcUdpLog,CUdpClient::UDP_SEND   ,_T(""),udpLogPrt, gStaticIP2);
	udpLogError = false;
	if( pSndUdpLog )
	{
		pSndUdpLog->Send(ptr,len);
		return pSndUdpLog->_GetLastError() ? true : false;
	}

	return false;
}

// Вывод HEX строки для заданного чичла байт
bool udpLogHex (BYTE *ptr, int len)
{
	char buf[MAX_PATH];
	//11.04.2011	
	//MakeHexStrANSII(ptr,len,buf, MAX_PATH - 1);
	MakeHexStrANSII(ptr,len,buf, MAX_PATH);
	return udpLog ((BYTE*)buf, strlen(buf));
}


// вывод юникод строки с предварительным преобразованием в 1251
//static TCHAR buf[MAX_PATH];
bool udpLog (TCHAR * ptr)
{
	//ZeroMemory(buf,MAX_PATH);
	char dst[MAX_PATH]; ZeroMemory(dst,sizeof(dst));
	int ret = WideCharToMultiByte(
				//04.05.2011 В эмуляторе нет поддержки Кириллицы,
				//поэтому вывод в лог текста на русском невозможен.
				//На КП все работает нормально.
				1251/*CP_ACP*/,					// code page
				NULL,					// performance and mapping flags
				ptr,					// wide-character string
				_tcslen(ptr),			// number of chars in string
				(char*)dst,				// buffer for new string
				MAX_PATH,				// size of buffer
				NULL,					// default for unmappable chars
				NULL);					// set when default char used

	return udpLog ((BYTE *)dst, ret);
}

//Получить версию файла
//Вход:
// - sFileName - имя файла или NULL для себя
//Выход:
// - номер версии
// - =-1 - ошибка
int GetFileVer(LPCTSTR sFileName)
{
	BYTE pBuf[1024];

	DWORD dwHandle;
	TCHAR sPath[MAX_PATH];
	if( !GetModuleFileName(NULL,sPath,MAX_PATH) )
		return -1;

	DWORD LenBuf = GetFileVersionInfoSize(sPath,&dwHandle);

	if( LenBuf == 0 )
		return -1;
	
	if( LenBuf > sizeof(pBuf) )
		LenBuf = sizeof(pBuf);

	memset(pBuf,0,sizeof(pBuf));

	if( !GetFileVersionInfo(sPath,dwHandle,LenBuf,pBuf) )
		return -1;

	UINT uSize = 0;
/*
	VS_FIXEDFILEINFO *ffi;

	DWORD dwSignature		;
	DWORD dwStrucVersion	;
	DWORD dwFileVersionMS	;
	DWORD dwFileVersionLS	;
	DWORD dwProductVersionMS;
	DWORD dwProductVersionLS;
	DWORD dwFileFlagsMask	;
	DWORD dwFileFlags		;
	DWORD dwFileOS			;
	DWORD dwFileType		;
	DWORD dwFileSubtype		;
	DWORD dwFileDateMS		;
	DWORD dwFileDateLS		;		

	if( VerQueryValue(pBuf,_T("\\"),(LPVOID*)&ffi,&uSize) )
	{
		dwSignature			= ffi->dwSignature; 
		dwStrucVersion		= ffi->dwStrucVersion;
		dwFileVersionMS		= ffi->dwFileVersionMS;
		dwFileVersionLS		= ffi->dwFileVersionLS;
		dwProductVersionMS	= ffi->dwProductVersionMS; 
		dwProductVersionLS	= ffi->dwProductVersionLS; 
		dwFileFlagsMask		= ffi->dwFileFlagsMask; 
		dwFileFlags			= ffi->dwFileFlags;	 
		dwFileOS			= ffi->dwFileOS; 
		dwFileType			= ffi->dwFileType; 
		dwFileSubtype		= ffi->dwFileSubtype; 
		dwFileDateMS		= ffi->dwFileDateMS; 
		dwFileDateLS		= ffi->dwFileDateLS; 
	}
*/
	TCHAR buf[256];
	LPCTSTR str; 

	if( !VerQueryValue(pBuf,_T("\\StringFileInfo\\040904b0\\FileVersion"),(LPVOID*)&str,&uSize) )
		return -1;
	
	int v1=-1,v2=-1,v3=-1,v4=-1;
	_stprintf(buf,_T("%s"),str);
	_stscanf(buf,_T("%d,%d,%d,%d"),&v1,&v2,&v3,&v4);

	return v4;
}

#ifdef DEF_COM_FIFO_OFF
//Получить из реестра базовый адрес СОМ-порта
//Вход:
// - nPort - номер СОМ-порта
//Выход: базовый адрес или 0-нет адреса
int GetComIOBase(int nPort)
{
	int iobase = 0;
	TCHAR szReg[256];
	_stprintf(szReg,_T("Drivers\\Builtin\\Serial%d"),nPort);

	TCHAR sHiveOld[16],sWorkOld[16];
	_tcscpy(sHiveOld,GetHiveName());
	_tcscpy(sWorkOld,GetKeyName());
	SetHiveName(_T(""));
	SetKeyName(_T(""));

	DWORD var = 0;
	if( ReadRegDword(szReg,_T("IOBase"),var,NULL,HKEY_LOCAL_MACHINE) )
		iobase = (int)var;

	SetHiveName(sHiveOld);
	SetKeyName(sWorkOld);

	_stprintf(szReg,_T("COM%d.IOBase=0x%x"),nPort,iobase);
	AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(LPARAM)szReg);

	return iobase;
}
#endif

/*
//Определяет активность связи отладочного порта на текущий момент
//Алгоритм:
//1.Если включен режим отладки ОТУ
//2.Если есть связь по отладочному порту и нет связи с ЦП
bool IsActivDbgPort()
{
	if( CRsDebug::bDbgOtu )
		return true;

	if( CRsDebug::bDbgPort && !pActivMdm->IsOK() && !pPassivMdm->IsOK() )
		return true;

	return false;

}
*/
//////////////////////////////////////////////////////////////////////////////
//АРМ ДНЦ,АРМ ДСП,СУ,РСУ,РУ(РУ.ВК,РУ.ОТ),ДУ

//Ф-я определяет состояние ТС СУ
bool IsSu()
{
	if( !IsSuDef() )										// если СУ неопределены
		return false;										// СУ отключен
	
	if( KP.IsTsOn(gSu[0],gSu[1]) == !gSu[2] )
		return false;
	return true;
}

//Ф-я определяет состояние ТС РСУ
bool IsRsu()
{
	if( !IsSuDef() )										// если РСУ неопределены
		return false;										// РСУ отключен

	if( KP.IsTsOn(gRsu[0],gRsu[1]) == !gRsu[2] )
		return false;
	return true;
}

//25.09.2015 Та-а-а-к!Вернули все взад - 1 сигнал РУ
//Ф-я определяет состояние режима РУ 
//Значение "включен" определяют gRu[2](по-умолчанию,1)
//Но учитывается возможность инверсии значения(включен, когда 0)
bool IsRu()
{
	if( !IsRuDef() )										// если РУ неопределен
		return false;										// РУ отключен

	if( KP.IsTsOn(gRu[0],gRu[1]) == !gRu[2] )				// РУ=!gRu[2]
		return false;										// режим РУ отключен
	return true;											// в любом другом варианте - включен
}
/*
//Ф-я определяет состояние режима РУ по 2-м ТС:РУ.ВК, РУ.ОТ.
//Значение "включен" определяют gRuVk[2] и gRuOt[2](по-умолчанию,1 и 0 соответственно)
//Но учитывается возможность инверсии значения(включен, когда 0 и 1)
//Возможные варианты состояний(по-умолчанию,когда gRuVk[2]=1 и gRuOt[2]=0):
// - РУ.ВК=0, РУ.ОТ=1 - РУ отключен
// - РУ.ВК=1, РУ.ОТ=0 - РУ включен - норма  
// - РУ.ВК=1, РУ.ОТ=1 - РУ включен - авария
// - РУ.ВК=0, РУ.ОТ=0 - РУ включен - авария
bool IsRu()
{
	if( !IsRuDef() )										// если РУ неопределены
		return false;										// РУ отключен

	if( KP.IsTsOn(gRuVk[0],gRuVk[1]) == !gRuVk[2] &&		// РУ.ВК=!gRuVk[2]
		KP.IsTsOn(gRuOt[0],gRuOt[1]) == !gRuOt[2]  )		// РУ.ОТ= gRuOt[2]
	{
		return false;										// режим РУ отключен
	}
	return true;											// в любом другом варианте - включен
}
*/

//Вариант, когда неопределен контроль одного из 2-х режимов "СУ","РУ":индикатор не отображается
//Ф-я возвращает текущий режим управления
int GetActRu()
{
	if( !IsRuDef() || !IsRsuDef() || !IsSuDef() )
		return MODE_NO;
	if( IsRu() )
		return MODE_RU;
	if( IsRsu() )
		return MODE_RSU;
	if( IsSu() )
		return MODE_SU;
	return MODE_DU;
}

//Контроль определения "СУ" в ini-файле
bool IsSuDef()
{
	if( gSu[0]<0 || gSu[1]<0 )
		return false;
	return true;
}

//Контроль определения "СУ" в ini-файле
bool IsRsuDef()
{
	if( gRsu[0]<0 || gRsu[1]<0 )
		return false;
	return true;
}

//25.09.2015 Та-а-а-к!Вернули все взад - 1 сигнал РУ
//Контроль определения "РУ" в ini-файле
bool IsRuDef()
{
	if( gRu[0]<0 || gRu[1]<0 )
	{
		return false;
	}
	return true;
}
/*
//Контроль определения "РУ" в ini-файле
bool IsRuDef()
{
	if( gRuVk[0]<0 || gRuVk[1]<0 ||			// РУ.ВК
		gRuOt[0]<0 || gRuOt[1]<0 )			// РУ.ОТ
	{
		return false;
	}
	return true;
}
*/

//Ф-я определяет наличие ДСП на станции
bool IsDsp()
{
	if( pPassivMdmDsp || pActivMdmDsp)
		return true;
	return false;
}

//Ф-я определяет режим управления при ДСП
bool IsRuDsp()
{
	if( IsDsp() && IsRu() )
		return true;
	
	return false;
}

//Ф-я определяет режим управления при ДСП
bool IsSuDsp()
{
	if( !IsDsp() )
		return false;
	
	if( IsRu() )
		return false;

	if( !IsRsu() && !IsSu() )
		return false;

	return true;
}

//Контроль разрешенных ТУ при СУ
//Вход:
// - tu - упакованное ТУ
bool IsTuForSu(WORD tu)
{
	DWORD tu1 = KP.GetExtractTu(tu);
	int m = tu1 ? KP.GetModulByTu(tu)	: 0;
	int k = tu1 ? KP.GetOutByTu(tu)		: 0;
	for( int i=0; i<gSuDncTu.n; i++ )
	{
		if( gSuDncTu.m[i] == m && gSuDncTu.tu[i] == k )
			return true;
	}
	return false;
}

//Контроль разрешенных ТУ при ДУ
//Вход:
// - tu - упакованное ТУ
bool IsTuForDu(WORD tu)
{
	DWORD tu1 = KP.GetExtractTu(tu);
	int m = tu1 ? KP.GetModulByTu(tu)	: 0;
	int k = tu1 ? KP.GetOutByTu(tu)		: 0;
	for( int i=0; i<gDuDspTu.n; i++ )
	{
		if( gDuDspTu.m[i] == m && gDuDspTu.tu[i] == k )
			return true;
	}
	return false;
}

//Определяет наличие связи на линиях с АРМ ДНЦ
bool IsDncOK()
{
	if( (pPassivMdm && pPassivMdm->IsOK()) || (pActivMdm && pActivMdm->IsOK()) )
		return true;
	return false;
}

//Определяет наличие связи на линиях с АРМ ДСП
bool IsDspOK()
{
	if( (pPassivMdmDsp && pPassivMdmDsp->IsOK()) || (pActivMdmDsp && pActivMdmDsp->IsOK()) )
		return true;
	return false;
}

//Определяет наличие связи на линиях с АРМ ДНЦ и статус управления(для индикатора)
int GetDncStatus()
{
	if( IsDncOK() )
	{
		//Связь есть,проверяем разрешение управления
		//Логика такая:
		// - если нет ДСП, сохраняем текущее поведение КП - не блокируем ТУ ни при каких условиях
		// - иначе включаем логику блокирования
		if( !IsDsp() )									// если нет ДСП,
			return MODE_STS_ACT_TU;						// все разрешаем

		//Есть ДСП
		if( IsRu() )									// если РУ,
			return MODE_STS_PAS_TU_;
		
		if( IsSu() || IsRsu() )							// если СУ или РСУ
		{
			if( gDu && gSuDncTu.n )						// и разрешены какие-то ТУ от ДНЦ
				return MODE_STS_PAS_TU;					// разрешено частичное управление
			else										
				return MODE_STS_PAS_TU_;				// запрет ТУ
		}

		//Это ДУ
		if( !gDu )										// если запрет ТУ
			return MODE_STS_ACT_TU_;
		return MODE_STS_ACT_TU;							// разрешены ТУ
	}
	return MODE_STS_OFFLINE;							// нет связи
}

//Определяет наличие связи на линиях с АРМ ДСП и статус управления(для индикатора)
int GetDspStatus()
{
	if( IsDspOK() )
	{
		//Связь есть,проверяем разрешение управления
		if( !IsRu() )									// если не РУ 
		{	
			if( IsSu() || IsRsu() )						// и СУ или РСУ
				return MODE_STS_ACT_TU;					// ДСП активен,ТУ разрешены
			if( gDuDspTu.n )							// это ДУ.Если разрешены ТУ от ДСП
				return MODE_STS_PAS_TU;					// разрешаем
		}
		return MODE_STS_PAS_TU_;						// блокируем ТУ
	}
	return MODE_STS_OFFLINE;							// нет связи
}

//
///////////////////////////////////////////////////////////////////////////////////////////

//Получить код ТУ из упакованной ТУ
int GetTuFromTu(DWORD tu)
{
	return (tu&MSK_TU_TU);
}

//Получить тип АРМ из упакованной ТУ
int GetArmFromTu(DWORD tu)
{
	return ((tu&MSK_TU_ARM) >> OFS_TU_ARM);
}

//Получить тип ЭЦ из упакованной ТУ
int GetEcFromTu(DWORD tu)
{
	return ((tu&MSK_TU_EC) >> OFS_TU_EC);
}

//Получить номер канала из упакованной ТУ(от 0)
int GetCanalFromTu(DWORD tu)
{
	return ((tu&MSK_CANAL) >> OFS_MPC_CANAL);
}

//Определяет активный АРМ на станции с/без учета DBG режима
// - bDbg - =true - с учетом DBG режима
int GetActiveArm(bool bDbg/*=true*/)
{
	if( bDbg && CRsDebug::bDbgTu )
		return ARM_DBG;

	if( IsDsp() )												// если есть ДСП
	{
		//В режиме РУ, при наличии ДСП, кто-то все равно должен слушать станцию,
		//сихронизировать время,осуществлять обмен со СПОК/УПОК и т.д.
		//Поэтому:
		// - если СУ или РСУ,считаем активным АРМ ДСП,несмотря на то,есть связь с ним или нет.
		//   В этих режимах нельзя на уровне КП принудительно сделать АРМ ДНЦ активным
		//   (к примеру, если нет связи с ДСП, то считать активным ДНЦ).
		//   Для этого нужно предварительно дать команду с АРМ ДНЦ на перевод станции в режим ДУ,
		//   которая всегда должна быть разрешена для ДНЦ(Агаджанов).
		//   Т.е.,КП является пассивным игроком,отслеживающим реальный режим на станции по соответствующим ТС.
		// - иначе,считаем активным АРМ ДНЦ.
		if( IsSu() || IsRsu() )									// если СУ или РСУ
		{
			//if( (pPassivMdmDsp && pPassivMdmDsp->IsOK()) || (pActivMdmDsp && pActivMdmDsp->IsOK()) )
				return ARM_DSP;
		}
	}
	
	return ARM_DNC;
}

//Наличие на станции ЭЦ МПЦ
bool IsMpc()
{
	if( pRSAcem[0] || pRSAcem[1] || pRSAcmpk[0] || pRSAcmpk[1] )
		return true;

	return false;
}

//11.05.2012 Добавил
//Ф-я определяет тип ЭЦ, для которого предназначена ТУ
int GetEc(WORD tu)
{
	if( pRSRpc )
		return TYPE_EC_RPC;

	int iMpc = GetMpc();
	//24.09.2012 Изменил
//	if( iMpc==TYPE_EC_MPK1)
//	{
//		if( pRSAcmpk[0] && tu>pRSAcmpk[0]->GetMinTu() && tu<pRSAcmpk[0]->GetMaxTu() )
//			iMpc = TYPE_EC_MPK1;
//		else
//		if( pRSAcmpk[1] && tu>pRSAcmpk[1]->GetMinTu() && tu<pRSAcmpk[1]->GetMaxTu() )
//			iMpc = TYPE_EC_MPK2;
//	}
	int iMtu = KP.GetRealMTU();
	if( iMpc && iMtu )											// это гибрид
	{
		if( KP.GetDelayFromTu(tu) == 0 )						// если нулевая пауза - ТУ МПЦ
			return iMpc;
	}
	else														// не гибрид
	{
		if( iMpc )
			return iMpc;
	}

	return TYPE_EC_MTU;
}


//Возврат актуального типа ЭЦ МПЦ
int GetMpc()
{
	if( pRSAcem[0] || pRSAcem[1] )
	{
		if( giEc == 0 )
			return TYPE_EC_EBL;
		if( giEc == 1 )
			return TYPE_EC_EM;
		if( giEc == 2 )
			return TYPE_EC_MZF;
	}

//	if( pRSAcmpk[0] )
//		return TYPE_EC_MPK;
//	if( pRSAcmpk[1] )
//		return TYPE_EC_MPK2;

	if( pRSAcmpk[0] || pRSAcmpk[1] )
		return TYPE_EC_MPK;

	return 0;
}

//Немодальный диалог вывода сообщения
class CDlgMsg *pDlgMsg = NULL;
void KpMessageBox(LPCTSTR sMsg,bool bClose)
{
	if( sMsg == NULL )
		return;

	if( pDlgMsg == NULL )
	{
		pDlgMsg = new CDlgMsg();
		if( pDlgMsg == NULL || !pDlgMsg->Create(NULL) )
		{
			TRACE(_T("Error. kp2004dlg. Can't create modeless dialog CDlgMsg!\r\n"));
			if( pDlgMsg )
			{
				delete pDlgMsg;
				pDlgMsg = NULL;
			}
			return;
		}
	}

	if( pDlgMsg && pDlgMsg->m_hWnd )
	{
		pDlgMsg->SetMsg(sMsg,bClose);

		if( !pDlgMsg->IsWindowVisible() )
			pDlgMsg->ShowWindow(SW_SHOW);
	}
}

#ifdef DEF_CMD_TIME_ZONE
//////////////////////////////////////////////////////////////
//19.12.2012 С командой запроса передаются настройки(LPVOID ptz) часового пояса
//в виде структуры REG_TZI_FORMAT, которая хранится в реестре в параметре
//"HKLM/Time Zones/Russian Standard Time/TZI".Если переданная в команде структура
//отличается от хранящейся в реестре КП,обновляем реестр.
//////////////////////////////////////////////////////////////
//Ф-я реализации запроса на изменение часового пояса
bool SetTimeZone(LPVOID ptz)
{
	if( !ptz )
		return false;

	bool bRet = false;

	//Сначала инициализируем рабочие ключи реестра
	HKEY hOldPreKey = GetHKEY();														//Сохраняем рабочие ключи
	TCHAR sOldHive[256], sOldKey[256];
	wcscpy( sOldHive, GetHiveName() );
	wcscpy( sOldKey, GetKeyName() );
	SetHKEY(HKEY_LOCAL_MACHINE);
	SetHiveName(_T(""));
	SetKeyName(_T(""));

	PREG_TZI_FORMAT p = (PREG_TZI_FORMAT) ptz;

	LPCTSTR sTZName = _T("Russian Standard Time");
	CString sActualKey,s1;
	sActualKey.Format(_T("Time Zones\\%s"),sTZName);
	
	REG_TZI_FORMAT rtz;
	DWORD dwSize = sizeof(REG_TZI_FORMAT);
	
	// считываем инфо для струтуры TIME_ZONE_INFORMATION из параметра "TZI"
	if( ReadRegBin(sActualKey,_T("TZI"),(PBYTE)&rtz,&dwSize) )
	{
/*		if( rtz.Bias != p->Bias )
		{
			rtz.Bias			= p->Bias;
			rtz.StandardBias	= p->StandardBias; 
			rtz.DaylightBias	= p->DaylightBias;
			memcpy((void*)&rtz.StandardDate,(void*)&p->StandardDate,sizeof(SYSTEMTIME));
			memcpy((void*)&rtz.DaylightDate,(void*)&p->DaylightDate,sizeof(SYSTEMTIME));
			WriteRegBin(sActualKey,_T("TZI"),(PBYTE)&rtz,dwSize);
			s1.Format(_T("(GMT+%.2d:00) Moscow, St. Petersburg, Volgograd"),abs(rtz.Bias/60));
			WriteRegText(sActualKey,_T("Display"),s1);
		}
*/
		if( memcmp(&rtz,p,dwSize) != 0 )
		{
			memcpy(&rtz,p,dwSize);
			WriteRegBin(sActualKey,_T("TZI"),(PBYTE)&rtz,dwSize);
			s1.Format(_T("(GMT+%.2d:00) Moscow, St. Petersburg, Volgograd"),abs(rtz.Bias/60));
			WriteRegText(sActualKey,_T("Display"),s1);

			TIME_ZONE_INFORMATION tzRu;
			memset((void*)&tzRu,0,sizeof(tzRu));
			tzRu.Bias			= rtz.Bias;
			tzRu.StandardBias	= rtz.StandardBias; 
			tzRu.DaylightBias	= rtz.DaylightBias;
			memcpy((void*)&tzRu.StandardDate,(void*)&rtz.StandardDate,sizeof(SYSTEMTIME));
			memcpy((void*)&tzRu.DaylightDate,(void*)&rtz.DaylightDate,sizeof(SYSTEMTIME));
			int len = _tcslen(sTZName);
			if( len >= sizeof(tzRu.StandardName)/sizeof(TCHAR) )
				len = sizeof(tzRu.StandardName)/sizeof(TCHAR) -1;
			_tcsncpy(tzRu.StandardName, sTZName, len);

			// берем DaylightName
			if( ReadRegText(sActualKey,_T("Dlt"),s1) )
			{
				len = s1.GetLength();
				if( len >= sizeof(tzRu.DaylightName)/sizeof(TCHAR) )
					len = sizeof(tzRu.DaylightName)/sizeof(TCHAR) -1;
				_tcsncpy(tzRu.DaylightName, s1, len);
			}

			//устанавливаем зону
			bRet = SetTimeZoneInformation(&tzRu) ? true : false;

			if( bRet && tzRu.DaylightBias!=0 )
			{
				//Вкл.авто перевод на летнее время
				DWORD dwAutoDst = 0;
				ReadRegDword(_T("Software\\Microsoft\\Clock"),_T("AutoDST"),dwAutoDst);
				if( dwAutoDst == 0 )
				{
					dwAutoDst = 1;
					WriteRegDword(_T("Software\\Microsoft\\Clock"),_T("AutoDST"),dwAutoDst);
				}
			}
		}
	}

	SetHKEY(hOldPreKey);
	SetHiveName(sOldHive);
	SetKeyName(sOldKey);

	return bRet;
}
#endif

//14.08.2015 
//Ф-я,определяющая АРМ с CПОК
int GetArmSpok()
{
	if( gUpok[ARM_DNC] == TYPE_SPOK )
		return ARM_DNC;
	if( gUpok[ARM_DSP] == TYPE_SPOK )
		return ARM_DSP;
	return -1;
}

//Ф-я,определяющая АРМ с УПОК
int GetArmUpok()
{
	if( gUpok[ARM_DNC] == TYPE_UPOK )
		return ARM_DNC;
	if( gUpok[ARM_DSP] == TYPE_UPOK )
		return ARM_DSP;
	return -1;
}

//Убрать CПОК в АРМ 
void ClearArmSpok()
{
	if( gUpok[ARM_DNC] == TYPE_SPOK )
		gUpok[ARM_DNC] = TYPE_NOTU;
	if( gUpok[ARM_DSP] == TYPE_SPOK )
		gUpok[ARM_DSP] = TYPE_NOTU;
	if( gUpok[ARM_DBG] == TYPE_SPOK )
	{
		if( gUpok[ARM_DNC] == TYPE_UPOK ||
			gUpok[ARM_DSP] == TYPE_UPOK )
		{
			gUpok[ARM_DBG] = TYPE_UPOK;
		}
		else
		{
			gUpok[ARM_DBG] = TYPE_NOTU;
		}
	}
}

//Убрать УПОК в АРМ 
void ClearArmUpok()
{
	if( gUpok[ARM_DNC] == TYPE_UPOK )
		gUpok[ARM_DNC] = TYPE_NOTU;
	if( gUpok[ARM_DSP] == TYPE_UPOK )
		gUpok[ARM_DSP] = TYPE_NOTU;
	if( gUpok[ARM_DBG] == TYPE_UPOK )
	{
		if( gUpok[ARM_DNC] == TYPE_SPOK ||
			gUpok[ARM_DSP] == TYPE_SPOK )
		{
			gUpok[ARM_DBG] = TYPE_SPOK;
		}
		else
		{
			gUpok[ARM_DBG] = TYPE_NOTU;
		}
	}
}

/*?*///25.08.2015 Добавил
//Ф-я определяет разрешение управления от указанного АРМ
bool IsForArm(int iArm,int& ak)
{
//	int aArm = GetActiveArm();

	if( CRsDebug::bDbgTu )							// мы в режиме отладки ОТУ,ТУ
	{
		if( iArm < ARM_DBG )						// если ТУ от ЦП или ДСП 
		{
			ak = ACK_TU_IGNR_DBG_OTU;				// ТУ отвергнута
			return false;
		}
	}
	else
	//контроль ТУ при ДНЦ
	if( iArm == ARM_DNC )							// если ТУ от ЦП и 
	{
		if( IsDsp() )								// если ДСП
		{
			if( !gDu )								// управление для ДУ запрещено
			{
				ak = ACK_TU_IGNR_DNC_DU;			// ТУ отвергнута
				return false;						// выход без выборки
			}
			if( IsRu() )							// и РУ 
			{
				ak = ACK_TU_IGNR_DSP_RU;			// ТУ отвергнута
				return false;						// выход без выборки
			}
			//01.10.2015.Отменил запрет управления от неактивного,т.к.для него возможно опциональное 
			//разрешение ограниченного числа ТУ
/*			if( iArm != aArm &&								// если указанный АРМ неактивный
				gUpok[iArm] == TYPE_SPOK )					// и на нем СПОК
			{
				ak = ACK_OTU_IGNR_ARM;						// команды(пакеты) СПОК для неактивного отвергаем всегда
				return false;
			}
*/
		}
	}
	else
	//контроль ТУ при ДСП
	if( iArm == ARM_DSP )							// если ТУ от ДСП
	{
		if( IsRu() )								// и РУ 
		{
			ak = ACK_TU_IGNR_DSP_RU;				// ТУ отвергнута
			return false;							// выход без выборки
		}
		//01.10.2015.Отменил запрет управления от неактивного,т.к.для него возможно опциональное 
		//разрешение ограниченного числа ТУ
/*		if( iArm != aArm &&								// если указанный АРМ неактивный
			gUpok[iArm] == TYPE_SPOK )					// и на нем СПОК
		{
			ak = ACK_OTU_IGNR_ARM;						// команды(пакеты) СПОК для неактивного отвергаем всегда
			return false;
		}
*/
	}

	return true;
}