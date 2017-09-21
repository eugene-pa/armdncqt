// Rpc.cpp: implementation of the CRpc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "kp2004.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
Изменения 2017.03.16
	ПРОБЛЕМА: перезапуск БМ1/БМ2. Есть ТУ перезапуска, однако ТУ перезапуска должна выдаваться конкретно
			  либо на БМ1, либо на БМ2, для чего используются биты А7,А6 поля адреса в пакете ТУ:
			    +-------- 
			    |    +--- 
	Первый байт	A7	A6	A5	A4	A3	A2	A1	A0
	Второй байт	B7	B6	B5	B4	B3	B2	B1	B0
	Третий байт	C7	C6	C5	C4	C3	C2	C1	C0
				A7,A6 = 00 - отправка на обе БМ
				A7,A6 = 01 - отправка на 1 БМ
				A7,A6 = 10 - отправка на 2 БМ
				A7,A6 = 11 - недопустиая комбинация

    РЕШЕНИЕ:  вводим две доп.виртуальные команды 
			  МОДУЛЬ = 0, ВЫХОД = 5		- ТУБМ1 - след.ТУ выдается на БМ1
			  МОДУЛЬ = 0, ВЫХОД = 6		- ТУБМ2 - след.ТУ выдается на БМ2
			  В этом случае перезапуск будет выполняться последовательностью:
			  - ТУБМ1 ВПБМ - перезапуск БМ2
			  - ТУБМ2 ВПБМ - перезапуск БМ1


Изменения 2013.07.17
	ПРОБЛЕМА: нужны задержки порядка 10 сек. Существующая реализация передавал задержку, в старших 4-х битах d15-d12 ТУ
	          аналогично штатным ТУ КП, но измеряемую в 0.5 сек, поэтому максимальная задержка была 7.5 сек
			  Требование иметь задержки по 0.5 было выставлено со стороны РПЦ Диалог

	РЕШЕНИЕ:  Кодирование IJ при передаче ТУ Диалог аналогично кодированию IJ ТУ КП:
			  TTTT.MMMM.MM.OO.OOOO, где TTTT - задержка, MMMMMM-модуль(6 бит),  OOOOOO-выход(6 бит)
			  Так как номер модуля в РПЦ Диалог кодируется 5-ю битами, имеем резерв - бит d11:
			  TTTT.tMMM.MM.OO.OOOO

	ВАЖНО:	  для совместимости с существующим ПО в этом бите передается старший бит пятибитной задержки
			  добавляющий к задержке в штатных битах d15-d12 8 сек.
			  Получаемое значение: t.TTTT

	ПРАВКИ:
			  - BYTE CRpc::rpcGetMdlFromTu  (WORD tu) 
			  - BYTE CRpc::rpcGetDelpcFromTu(WORD tu) 
			  - void CRpc::MakeRpcDlgTu (WORD tu, BYTE * ptr)
			  - в комментариях подробно описано кодирование ТУ РПЦ Диалог
			  - подпралены комментарии
-------------------------------------------------------------------------------------------------------
  Изменения 2008.10.07
	- Rpc.cpp, Rpc.h
		- ввел переменные bOtu1, bOtu2
		- ввел обработку виртуальных команд РПЦ (модуль = 0)
			МОДУЛЬ = 0, ВЫХОД = 3   - "РПЦ-ОТУ-1"
			МОДУЛЬ = 0, ВЫХОД = 4	- "РПЦ-ОТУ-2"
		- рекомендовал нумеровать вирт.команду ПАУЗА c j=10
-------------------------------------------------------------------------------------------------------
  Изменения 2008.09.25
	- Rpc.cpp, Rpc.h
		- ввел переменную bNextTuSolo
-------------------------------------------------------------------------------------------------------
  Изменения 2008.09.24, связанные с реализацией ТУ РПЦ
	- Rpc.cpp, Rpc.h
		- сделал MakeRpcDlgTu членом класса CRpc
		- ввел функции rpcGetOutFromTu, rpcGetMdlFromTu, rpcGetDelpcFromTu
		- ввел обработку виртуальных команд РПЦ (модуль = 0)
			МОДУЛЬ = 0, ВЫХОД = 1    - "ГРУППА ТУ"   две следующие команды в один пакет 
			МОДУЛЬ = 0, ВЫХОД = 2    - "С ВЫДЕРЖКОЙ" до окончания исполнения след.команды ТУ блокируются
			МОДУЛЬ = 0, ВЫХОД = 3    - "РПЦ-ОТУ-1",
			МОДУЛЬ = 0, ВЫХОД = 4    - "РПЦ-ОТУ-2",
			МОДУЛЬ = 0, ВЫХОД = 11...- "ПАУЗАx"      - задержка в цикле выдачи ТУ на указанное время

		- ввел константу DefaultBetweenTu = 500 - задержка между командами ТУ по умолчанию
		- ввел переменную DWORD tickBetweenTu = 500;// задержка между командами ТУ. 
		  По умолчанию 500 мс, может меняться на один такт виртуальной командой 
		  ПАУЗАx (модуль = 0, выход > 1)
		- ввел nGroupTu	- признак группировки n следующих ТУ в один пакет
-------------------------------------------------------------------------------------------------------
  Изменения 2008.09.15, связанные с реализацией ТУ РПЦ
	- Rpc.cpp
		- переменная DWORD	tickLastTuSent - засечка времени отправки ТУ
		- функция bool MakeRpcDlgTu (WORD tu, BYTE * ptr) - формирование кода ТУ
		- раскомментирован и изменен цикл while функции CRpc::PollingLine()
		- сделан вывод лога udpLogHex
		- в конце файла добавлены 
		  статические переменные 
			const int udpLogPrt = 7010;	- UDP порт вывода лога в сеть
			bool  udpLogError = false;	- признак наличие ошибки при выводе лога в сеть
		  функции 
			void MakeHexStrANSII(BYTE * pBufIn,int szIn,char * pBufOut, int szOut);
			bool udpLog (BYTE *ptr, int len);
			bool udpLogHex (BYTE *ptr, int len);
			bool udpLog (CString s);
			void StrRpcTu (WORD tu, LPTSTR buf, int buflen)

	- НАДО:
		- проверить как отображаются команды ТУ в списке полученных, видимо надо сделать
		  более корректным отображение кода ТУ: StrRpcTu (WORD tu, LPCTSTR *buf, int buflen)
		- добавить отображение исполненной команды ТУ в списке исполненных команд
		  И udpLog строки ТУ
		- перенести объявления в другое место
			void MakeHexStrANSII(BYTE * pBufIn,int szIn,char * pBufOut, int szOut);
			bool udpLog (BYTE *ptr, int len);
			bool udpLogHex (BYTE *ptr, int len);
			bool udpLog (CString s);
			void StrRpcTu (WORD tu, LPTSTR buf, int buflen);


*/

BYTE TsStatusRpc   [RpcDlgPacker::MAX_LEN];	// массив состояний ТС(РПЦ)
WORD TsRpcSize = 0;							// размер блока ТС


// Массив состояний Диагностики РПЦ. Реальная длина DiagRpcSize = 4 + 6 * число_шкафов
// Первые 4 байта отводятся для информации по работающим БМ в шкафах (макс. - 4 шкафа)
//        В них храним A7,A6 - номер БМ, от которой получили
// Следующие 6 * nБМ байт - по 6 байт первых диагностических групп по каждому шкафу
//        Реально диагностические группы у нас нигде не привязаны 
const int ourDiagLen = 4;					// резервируем 4 байта под 4 шафа всегда
BYTE DiagStatusRpc [RpcDlgPacker::MAX_LEN];	// буфер накопления диагностики
WORD DiagRpcSize = ourDiagLen;				// здесь считаем общий размер блока Диагностики РП 4 + 6 * число_шкафов


// массив сообщений, испоьзуется в CRpc::PollingLine()
static PCHAR sMsg1[] = 
{
	"РПЦ.Сброс режима виртуальной команды по тайм-ауту",				// 0  MSG_CLEAR   
	"РПЦ.ВИРТ: ГРУППА ТУ",												// 1  MSG_GROUP   
	"РПЦ.ВИРТ: С ВЫДЕРЖКОЙ",											// 2  MSG_SOLO    
	"РПЦ.ВИРТ: РПЦ-ОТУ-1",												// 3  MSG_OTU1    
	"РПЦ.ВИРТ: РПЦ-ОТУ-2",												// 4  MSG_OTU2    
	"РПЦ.ПАУЗА ",														// 5  MSG_PAUSE   
	"РПЦ.ТУ выполняется в изолированном режиме с выдержкой ",			// 6  MSG_WAITING 
	"РПЦ.ВИРТ: вывод ТУ на БМ1",										// 7  MSG_BM1	
	"РПЦ.ВИРТ: вывод ТУ на БМ2 ",										// 8  MSG_BM2	
};

// 2017.03.16. использую перечисление для работы с индексами сообщений
enum
{
	MSG_CLEAR   = 0,
	MSG_GROUP   ,
	MSG_SOLO    ,
	MSG_OTU1    ,
	MSG_OTU2    ,
	MSG_PAUSE   ,
	MSG_WAITING ,
	MSG_BM1		,
	MSG_BM2		,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Глобальные статические списки, определяющие конфигурацию РПЦ: колическтво шкафов, адреса станции и шккафов
// Конфигурация задается так: 
// РПЦДИАЛОГ=COM6(9600,N,8,1),БМ06361101:64,БМ063611012:64

CDWordArray CRpc::arAddrBmEx;				// адреса БМ, имеющие место быть; заполняется при чтении INI файла
											// 0 - 0x06361001	- Кашпир, шеаф 1
											// 1 - 0x06361101	- Кашпир, шеаф 2
CWordArray  CRpc::arTsGroups;				// массив числа групп ТС, индексированный по порядковому номеру БМ 
											// 0 - 64
											// 1 - 64
CWordArray  CRpc::arTsOffset;				// массив смещений групп ТС, индексированный по порядковому номеру БМ 
											// 0 - 0
											// 1 - 64
//11.04.2011 Добавил
CDWordArray  CRpc::arTimeLast;				// массив временных точек для каждого БМ

static TCHAR tBuf[MAX_PATH];				// буфер для форматирования строк

CRpc::CRpc(CWnd* pWnd,LPCTSTR Port,LPCTSTR szInit) : 	
	CRS232(pWnd,Port,szInit)
{
	rpcLastError	= 0;					// код ошибки
	IndxPolling		= -1;					// индекс в массиве адресов БМ цикла опроса
	bShowDetail		= true;					// детальное отображение статуса прм/прд

	tickLastTuSent	= 0;					// время в тиках последней отправки ТУ
	tickBetweenTu	= DefaultBetweenTu;		// задержка между командами ТУ. По умолчанию 500 мс, может меняться
	nGroupTu		= 0;					// признак группировки n следующих ТУ в один пакет
	bNextTuSolo		= false;				// до окончания след. команды блокировать ТУ
	bOtu1			= false;				// фоаг, взводисый виртуальной командой "РПЦ-ОТУ-1"
	bOtu2			= false;				// фоаг, взводисый виртуальной командой "РПЦ-ОТУ-2"
	bmMask			= 0;

	m_bAckTout		= false;

	ClearCounters ();						// очистка статистики
											// Инициализируем COM портов перенесена 03.12.2012 в OnInitDialog 
											// для контроля вызова Init()
}


CRpc::~CRpc()
{
}


// Описание формирования трехбайтного кода ТУ по закодированной двухбайтной посылке IJ для РПЦ ДИАЛОГ
// ВАЖНО: начиная с 2013.07.17 для организации пятибитной задержки в 500мс интервалах (до 15.5 сек)
//        старший бит D11 традиционного поля модуля используется в качестве старшего бита задержки в 
//		  дополнение к традиционным битам D15-D12
//  Код IJ:
//==fedc ba98  7654 3210==
//  TTTT.*MMM  MMoo.oooo
//	  где:
//	OOOOOO  - номер выхода (макс. 63)
//	MMMMM   - номер модуля (макс. 31)  
//	*TTTT   - время исполнения ТУ в интервалах 0.5 сек (макс.15.5)

// ВИРТУАЛЬНЫЕ КОМАНДЫ:
//	МОДУЛЬ = 0, ВЫХОД = 1    - ВИРТ "ГРУППА ТУ"   две следующие команды в один пакет 
//	МОДУЛЬ = 0, ВЫХОД = 2    - ВИРТ "С ВЫДЕРЖКОЙ" до окончания исполнения след.команды ТУ блокируются
//	МОДУЛЬ = 0, ВЫХОД = 3    - ВИРТ "РПЦ-ОТУ1"    след.ТУ будет с признаком первая часть ОТУ
//	МОДУЛЬ = 0, ВЫХОД = 4    - ВИРТ "РПЦ-ОТУ2"    след.ТУ будет с признаком вторая часть ОТУ
//  МОДУЛЬ = 0, ВЫХОД = 5	 - ВИРТ "ТУБМ1"		  след.ТУ выдается на БМ1
//  МОДУЛЬ = 0, ВЫХОД = 6	 - ВИРТ "ТУБМ2"		  след.ТУ выдается на БМ2
//	МОДУЛЬ = 0, ВЫХОД = 11...- ВИРТ "ПАУЗАx"      задержка в цикле выдачи ТУ на указанное время

// Первый байт	ТУ:  A7	 A6	 A5	 A4	 A3	 A2	 A1	 A0
// Второй байт	ТУ:  B7	 B6	 B5	 B4	 B3	 B2	 B1	 B0
// Третий байт	ТУ:  C7	 C6	 C5	 C4	 C3	 C2	 C1	 C0
/*
Где:
A7, A6  -  тип команды управления (в настоящее время в увязке реализовано только управление выходами)
1) прямое управление выходами
-----------------------------
A7, A6  =  00 –  команда управления выходами БМ-1602:  
A5…A0 – порядковый номер выхода;
B7…B5 – признак команды управления:
B7…B5 = 000 – простая команда, выполнение следующей команды параллельно с данной;
B7…B5 = 100 – простая команда, выполнение следующей команды по окончании времени действия данной;
B7…B5 =  001 – первая часть ответственной команды;
B7…B5 =  010 – вторая часть ответственной команды;
B4…B0 – Адрес модуля выходов (см.ниже)
В4-В1 - старшее 16-ричное число
В0    – младшее (0 или 1)
С7…С0 – значение времени (дискретность 0,1 сек), в течении которого необходимо воздействовать на объект управления через соответствующий выход модуля;
C7…C0 = 0FFh – включить выход постоянно;
C7…C0 = 00h – выключить выход;

2) задержка
-----------
При A7..A0 = 00000000 и B7..B0 = 10000000 управление выходами не происходит, C7..C0 – определяет время задержки выполнения следующей команды.

3) маршрутное управление
------------------------
A7, A6  =  01 –  команда маршрутного управления:  
A5…A0  –   категория маршрута;
B7…B0, C7…C0  – номер маршрута;

4) сервисные команды
--------------------
A7, A6  =  10 –  сервисная команда управления:  
A5…A0 = 000000  –   резерв;
B7…B0, C7…C0  – служебная команда. 

5) резерв
---------
A7, A6  =  11 –  резерв.
*/

// формирование поля номера модуля вывода с учетом особенностей перекодировки
// Разъяснения к протоколу от С.Григорьева: 
// В В4-В1 записывается старшее 16-ричное число номера модуля из таблицы ТУ, в В0 – младшее (0 или 1).                 ============  
// Проще говоря, из проектного номера модуля D7-D0 нужно убрать D3-D1 со сдвигом вправо: XXXX.---X
// ТАКИМ ОБРАЗОМ: В РПЦ ДИАЛОГ ДОСТАТОЧНО 5 БИТОВ ДЛЯ ПЕРЕДАЧИ НОМЕРА МОДУЛЯ
//
// Номера модулей в БД кодируются так: младший полубайт - старший байт проектного номера, бит D4 - 0/1 проектного номера модуля
//    РПЦ		0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0,		
//	  БД-10		   1	 2     3     4     5     6     7     8     9  	10    11    12    13    14
//    БД-16     0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0a  0x0b  0x0c  0x0d  0x0e		  
//    Протокол: 0x02  0x04  0x06  0x08  0x0a  0x0c  0x0e  0x10  0x12  0x14  0x16  0x18  0x1a  0x1c		

//    РПЦ		0x11, 0x21, 0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xa1, 0xb1, 0xc1, 0xd1, 0xe1,		
//    БД-10       17    18    19    20    21    22    23    24    25    26    27    28    29    30
//    БД-16     0x11  0x12  0x13  0x14  0x15  0x16  0x17  0x18  0x19  0x1a  0x1b  0x1c  0x1d  0x1e
//    Протокол: 0x03  0x05  0x07  0x09  0x0b  0x0d  0x0f  0x11  0x13  0x15  0x17  0x19  0x1b  0x1d		


// 2013.07.16. выделение номера модуля в формате НСИ ДЦ из кода IJ (биты d10-d6)
BYTE CRpc::rpcGetMdlFromTu  (WORD tu) 
{												// TTTT T
	return (tu & 0x07c0) >> 6;					// ****.*МММ.ММ**.**** - берем только 5 бит, старший - под старший бит задержки
}												// fedc.ba98.7654.3210     


// Получить проектный номер модуля по номеру модуля НСИ ДЦ
BYTE CRpc::rpcGetProektMdl(BYTE m)
{
	return ((m & 0x0F) << 4) | ((m & 0xF0) >> 4);
}


// выделить задержку ТУ из команды ТУ (в 100мс единицах)
// 2013.07.16. Бит d11 используется для расширения разрядности задержки в качестве СТАРШЕГО бита
//             Таким образом под задержку тспользуется 5 битов с шагом 500мс.Максимальная задержка = 15,5 сек
BYTE CRpc::rpcGetDelpcFromTu(WORD tu) 
{ 
	byte delay = (tu & 0xf000) >> 12;			// выделяем младшие 4 бита задержки
	if (tu & 0x0800)							// если установлен бит d11
		delay |= 0x10;							// добавляем старший бит задержки
	return (delay * 10 ) / 2;					// преобразуем единицы измерения 0.5 сек в 100мс. 
}

// Форматировать номер модуля НСИ ДЦ для передачи в РПЦ 
// Универсальный алгоритм учитывающий особенности - проектного кодирования модулей РПЦ,
//												  - кодирования модулей в НСИ ДЦ 
//												  -	формата передачи номера в протоколе
// m - номер модуля в формате НСИ ДЦ
BYTE CRpc::rpcGetFormatMdl(BYTE m)
{
	BYTE lo = m & 0x10 ? 1 : 0;						// младший разряд В0 адреса модуля РПЦ (0/1) берем из бита D4 модуля НСИ
	byte hi = (m & 0x0f) << 1;						// старший полубайт сдвигаем в позицию В4-В1
	return (hi | lo);								// добавляем младший бит
}


// Форматирование кода ТУ в соответствии с протоколом увязки
// tu  - код ТУ в формате НСИ ДЦ
// ptr - укзатель на буфер для записи сформатированных данных
void CRpc::MakeRpcDlgTu (WORD tu, BYTE * ptr)
{
	// 1. Номер выхода напрямую; A7,A6 - тип ТУ = 00 (управление выходами)
	//    При необходимости выполнения других типов - менять тип здесь с учетом:
	// A7, A6  =  00 –  команда управления выходами БМ-1602 (РЕАИЗОВАНО!)  
	// A7..A0  =  00000000 и B7..B0 = 10000000 управление выходами не происходит, C7..C0 – определяет время задержки выполнения следующей команды.
	// A7, A6  =  01 –  команда маршрутного управления:  
	// A7, A6  =  10 –  сервисная команда управления:  
	// A7, A6  =  11 –  резерв.
	* ptr++ = rpcGetOutFromTu(tu);					// tu & 0x3f;		

	// 2. Выделение номера модуля в формате НСИ ДЦ из кода IJ (биты d10-d6)
	BYTE mdl = rpcGetMdlFromTu(tu);	

	// 3. Форматирование номера модуля в формат B4...B0 пакета ТУ РПЦ Диалог
	// формирование расширенных номеров - универсальное (для Разъезд-1 не имеет значения)
	mdl = rpcGetFormatMdl(mdl);

	// 3. Формируем биты B7...B5 
	// B7...B5  = 000 - Обычная ТУ
	// B7...B5  = 100 - Обычная ТУ с выдержкой: след ТУ не будут выполняться до окончания актуальной
	//                  (не задействована, хотя можно задействовать ВИРТ "С ВЫДЕРЖКОЙ" взамен ожиданию в коде)  
	// B7...B5  = 001 - первая часть ОТУ
	// B7...B5  = 010 - вторая часть ОТУ

	// Если были вирт.команды РПЦ-ОТУ1 или РПЦ-ОТУ2 - добавить признаки первой или второй части ОТУ
	// если установлены флажки ОТУ bOtu1, bOtu2 - проставить соответствующий признак ОТУ
	if (bOtu1)
		mdl |= 0x20;								// с признаком первая часть ОТУ
	if (bOtu2)
		mdl |= 0x40;								// с признаком вторая часть ОТУ
	bOtu1 = bOtu2 = false;							// сбрасываю 

	* ptr++ = mdl;

	// 4. Время в сек
	* ptr++ = rpcGetDelpcFromTu(tu);				// ((tu & 0xf000) >> 12) * 10;
	
}



// Реализация протокола циклического информационного обмена КП с РПЦ
// Выполняется в отдельном потоке ThreadRpc (ThreadRpc.cpp)
// 11.03.2011 Вариант с выводом в лог и архив при установке соответствующих флагов правой кнопкой на панели РПЦ
void CRpc::PollingLine()
{
	if (arAddrBmEx.GetSize()==0)
	{
		AfxMessageBox (_T("BM list is empty..."));		// оставляем для облегчения конфигурирования
		return;
	}

	// Формируем фиксированные длины блоков ТС и ДИАГ и строку конфигурации по описанию интерфейса с РПЦ Диалог
	CString sMsg,sTmp;									// Можно использовать CString в случае однократного вызова при инициализации
	for (int i=0; i<arTsGroups.GetSize(); i++)
	{
		TsRpcSize   += arTsGroups[i];					// считаем общее число групп ТС (во 16 бит)
		DiagRpcSize += RpcDlgPacker::DIAG_LEN * 2;		// выделяем по 6 байт (2*DIAG_LEN) диагностической информации РПЦ на каждый шкаф

		sTmp.Format(_T("БМ%X:%d  "), arAddrBmEx[i],arTsGroups[i]);
		sMsg += sTmp;
	}
	TsRpcSize *= 2;										// получаем длину в байтах

	m_pWnd->SetDlgItemText(IDC_STATIC_RPC_CONFIG,sMsg);	// отображаем строку конфигурации в диалоге

	CSingleLock TuListLock(&TuListLocker);

	
	// возможно, надо иметь отдельную очередь для РПЦ
	CSingleLock TuLock(&TuLocker);						// синхронизатор доступа к очереди ТУ

	TCHAR buf1[MAX_PATH];								// буфер для формирования сообщений лога
	WORD tu = 0;
	DWORD tuCod = 0;

	// каждые 100 мс выполняем обмен
	while (WaitForSingleObject(CRS232::ghThreadExitEvent,100)==WAIT_TIMEOUT)
	{
		char buf[MAX_PATH];											// буфер сообщений

		WORD tuBuf[16 * 3];											// буфер формирования ТУ для передачи
		BYTE * pData = (BYTE *)tuBuf;								// указатель в буфере
		int  nTu = 0;												// число ТУ для передачи

		gtThRpcLast = gTimeCnt;										// Контроль работы потока по засечкам времени 
																	// 18.09.2012 Изменил, использую счетчик времени КП, было gtThRpcLast = CTime::GetCurrentTime().GetTime();
		if( !KP.IsControlOne() )									// связь только при активности блока
		{
			Sleep(500);												// ждем подключения к линии
			continue;
		}
		
		// Опрашиваем поочередно все БМ, адреса которых описаны в массиве arAddrBm
		// Для этого используем индекс в массиве IndxPolling
		if (IndxPolling < 0 || IndxPolling >= arAddrBmEx.GetSize())
			IndxPolling = 0;
		DWORD addrBM = arAddrBmEx[IndxPolling];						// адрес очередной БМ для опроса						

		// Здесь надо обеспечить выдержку паузы между командами tickBetweenTu (по умолчанию 0.5 сек)
		// Если есть ТУ для БМ, адрес БМ меняется в соответствии с ТУ

		if (	GetTickCount() - tickLastTuSent >= tickBetweenTu	// прошло время паузы ?
			&&	HowManyToDo())										// есть команды ТУ    ?
		{
			tickBetweenTu = DefaultBetweenTu;						// сброс задержки в значение по умолчанию
			if (TuLock.Lock())
			{
				nGroupTu = 0;
				tu = 0;
				tuCod = 0;

				// выбираем ТУ из очереди и формируем список ТУ для передачи в РПЦ с контролем переполнения буфера tuBuf
				while (nTu < sizeof(tuBuf)/sizeof(WORD))			
				{
					if (TuListLock.Lock())							// Блокируем очередь и извлекаем очередную команду
					{
						//12.05.2012 Изменил  с учетом ТУ DWORD		//tu = GetToDo();
						//13.06.2013 Изменил с учетом контроля ТС	//tuCod = GetToDo();											// выбираем ТУ
						WORD ts = 0;
						tuCod = GetToDo(ts);						// выбираем код ТУ 32 бита
						tu = GetTuFromTu(tuCod);					// ТУ - младшие 16 бит
					}
					TuListLock.Unlock();
					
					if( !tu )										// команда с кодом 00 - недопустимая; игннорируем
					{
						bNextTuSolo = bOtu1 = bOtu2 = false;		// сброс флагов вирт.ТУ
						bmMask = 0;
						nGroupTu = 0;
						if( LogArh[SYS_RPC].log ) 
						{
							udpLog((PBYTE)sMsg1[MSG_CLEAR],strlen(sMsg1[MSG_CLEAR]));	// "РПЦ.Сброс режима виртуальной команды по тайм-ауту"
						}
						break;
					}

					// вычисляем номер модуля и отдельно отрабатываем виртуальные команды с номером модуля = 0
					byte noMdl = rpcGetMdlFromTu (tu);
					byte noOut;
					// ======================================================================================
					if (noMdl == 0)
					{	
						// 1. Обработать виртуальные команды
						//	ВЫХОД = 1    - "ГРУППА ТУ"   две следующие команды в один пакет 
						//	ВЫХОД = 2    - "С ВЫДЕРЖКОЙ" до окончания исполнения след.команды ТУ блокируются
						//	ВЫХОД = 3    - "РПЦ-ОТУ-1"
						//	ВЫХОД = 4	 - "РПЦ-ОТУ-2"
						//  ВЫХОД = 5	 - "ТУБМ1"		 след.ТУ выдается на БМ1
						//  ВЫХОД = 6	 - "ТУБМ2"		 след.ТУ выдается на БМ2
						//	ВЫХОД = 11...- "ПАУЗАx"      задержка в цикле выдачи ТУ на указанное время

						noOut = rpcGetOutFromTu (tu);
						// МОДУЛЬ = 0, ВЫХОД = 1 - ГРУППА ТУ две следующие команды в один пакет 
						switch (noOut)
						{
							case 1:									// 0:1  "ГРУППА ТУ"
							{
								if( LogArh[SYS_RPC].log ) 
									//sprintf (buf,"ВИРТ: ГРУППА ТУ");
									udpLog((PBYTE)sMsg1[MSG_GROUP],strlen(sMsg1[MSG_GROUP]));

								nGroupTu = 1;						// если nGroupTu >0 - не выходим после Make
								continue;
							}
							case 2:									// 0:2  "С ВЫДЕРЖКОЙ"
							{
								bNextTuSolo = true;
								if( LogArh[SYS_RPC].log ) 
									//sprintf (buf,"ВИРТ: С ВЫДЕРЖКОЙ");
									udpLog((PBYTE)sMsg1[MSG_SOLO],strlen(sMsg1[MSG_SOLO]));
								continue;
							}
							case 3:									// 0:3  "РПЦ-ОТУ-1"
							{
								bOtu1 = true;
								if( LogArh[SYS_RPC].log ) 
									//sprintf (buf,"ВИРТ: РПЦ-ОТУ-1");
									udpLog((PBYTE)sMsg1[MSG_OTU1],strlen(sMsg1[MSG_OTU1]));
								continue;
							}
							case 4:									// 0:4  "РПЦ-ОТУ-2"
							{
								bOtu2 = true;
								if( LogArh[SYS_RPC].log ) 
									//sprintf (buf,"ВИРТ: РПЦ-ОТУ-2");
									udpLog((PBYTE)sMsg1[MSG_OTU2],strlen(sMsg1[MSG_OTU2]));
								continue;
							}
							case 5:									// 0:5 "ТУБМ1"
							{
								bmMask = 0x40;
								if( LogArh[SYS_RPC].log ) 
									udpLog((PBYTE)sMsg1[MSG_BM1],strlen(sMsg1[MSG_BM1]));
								continue;
							}
							case 6:									// 0:6 "ТУБМ1"
							{
								bmMask = 0x80;
								if( LogArh[SYS_RPC].log ) 
									udpLog((PBYTE)sMsg1[MSG_BM2],strlen(sMsg1[MSG_BM2]));
								continue;
							}
							// МОДУЛЬ = 0, ВЫХОД > 1 - задержка в цикле выдачи ТУ на указанное время
							default:								// 0:3 и т.д. - "ПАУЗА"
							{
								tickBetweenTu = rpcGetDelpcFromTu(tu) * 100;

								if( LogArh[SYS_RPC].log ) 
								{
									//"Пауза"
									sprintf(buf,"%s%d мс",sMsg1[MSG_PAUSE],tickBetweenTu);
									udpLog((BYTE *)buf, strlen (buf));
								}
								break;
							}
						}
					}	

					// Если это была команда ПАУЗА и в пакете еще нет команд, выходим из цикла!
					if (noMdl==0 && noOut > 2 && nTu==0)
						break;
					// ======================================================================================

					// если с помощью вирт.команды "С ВЫДЕРЖКОЙ" был установлен флаг bNextTuSolo
					// кстанавливаем время между командами ТУ оавным времени исполнения текущей команды
					if (bNextTuSolo)
					{
						tickBetweenTu = rpcGetDelpcFromTu(tu) * 100;
						if( LogArh[SYS_RPC].log ) 
						{
							//sprintf (buf,"ТУ выполняется в изолированном режиме с выдержкой %d мс",tickBetweenTu);
							sprintf (buf,"%s%d мс",sMsg1[MSG_WAITING],tickBetweenTu);
							udpLog((BYTE *)buf, strlen (buf));
						}
						bNextTuSolo = false;
					}

					// Отобразить исполненню команду в списке DONE
					// (записываем команду в очередь исполненных команд)
					if (TuListLock.Lock())									// Блокируем очередь
					{
						//12.05.2012		
						//PutDone(tu,CTime::GetCurrentTime().GetTime());
						PutDone(tuCod,CTime::GetCurrentTime().GetTime());
						//Надо послать квитанцию
						PutAckTu(tuCod,ACK_TU_TODO);
					}
					TuListLock.Unlock();

					// Отправить сообщение диалогу для перерисовки очереди команд.
					AfxGetMainWnd()->SendMessage(WM_REFRESH_TULIST,1,0);	// WPARAM = 1 - обновить все(обе) очереди

					// Вывести в лог параметры  ТУ (группу, выход, время) в виде строки
					if( LogArh[SYS_RPC].log ) 
					{
						StrRpcTu (tu, buf1, MAX_PATH);
						udpLog(buf1);
					}

					// Сформировать блок ТУ, если команда не виртуальная
					if (noMdl)
						MakeRpcDlgTu (tu,pData + nTu++ * RpcDlgPacker::TU_LEN);

					// если нет счетчика шрупповых команд - выход
					if (nGroupTu==0)
						break;
					nGroupTu --;											// декремент счетчика

					// Пока выбираем одну команду ТУ, чтобы не было конфликта разных адресов БМ в разных ТУ
					// По хорошему надо выбрать все ТУ для одного и того же адреса БМ используя ShowToDo(i)
				}
				TuLock.Unlock();
			}
		}

		// если были не виртуальные ТУ - лог
		if( nTu )
		{
			// Вывести в лог сформированный блок ТУ в 16-ричном виде
			tickLastTuSent = GetTickCount();
			if( LogArh[SYS_RPC].log ) 
				udpLogHexRpc (pData, nTu * RpcDlgPacker::TU_LEN,true);
		}

		// Формируем пакет для передачи и отправляем его в РПЦ
// УТОЧНИТЬ addrBM | bmMask
		RpcDlgPacker pack (addrBM | bmMask, RpcDlgPacker::TYPE_TU, pData, nTu * RpcDlgPacker::TU_LEN);
		Send (&pack, pack.GetFullLen());

		//if( nTu )	//16.03.2011 Выводим все в лог и архив
		{
			//09.03.2011 Изменил: опциональный вывод в лог и архив
			if( LogArh[SYS_RPC].log ) 
				udpLogHexRpc ((BYTE *)&pack, pack.GetFullLen(),true);
			if( LogArh[SYS_RPC].arh )
				PutToArchive((BYTE *)&pack, pack.GetFullLen(),true);
		}
		nSntBt += pack.GetFullLen();
		nSntPk ++;
		
		// уведомляем окно
		if (m_pWnd)
			m_pWnd->SendMessage(WM_USER_RPC_SEND,0,(LPARAM)&pack);
		
		// принимаем ответ от РПЦ и обрабатываем ТС
		if (GetDataFromLine())
		{
			ExtractTs();				// извлекаем ТС и записываем их в буфер
		}
		else
		{
			if( rpcLastError < 0 )
			{
				nErrors++;
				DiagStatusRpc[IndxPolling] = 0;
				if (m_pWnd && bShowDetail)					
					//m_pWnd->SendMessage(WM_USER_RPC_ERROR,(WPARAM)LenIn,(LPARAM)&pack);
					m_pWnd->SendMessage(WM_USER_RPC_ERROR,(WPARAM)LenIn,(LPARAM)&bufIn);
				//04.03.2011 Незя,т.к в блоке диагностики передается диаг от Rpc
				//SendAckErr(rpcLastError);
				if( LogArh[SYS_RPC].log ) 
				{
					_stprintf(buf1,_T("ОШ.РПЦ.%s"),RpcDlgPacker::GetErrorText(rpcLastError));
					udpLog(buf1);
				}
			}
		}
		//09.03.2011 Изменил: опциональный вывод в лог и архив
		if( (LogArh[SYS_RPC].log || LogArh[SYS_RPC].arh) && LenIn ) 
		{
			if( LogArh[SYS_RPC].log )
			{
				_stprintf(buf1,_T("РПЦ->"));
				MakeHexStr(bufIn,LenIn,buf1+_tcslen(buf1),sizeof(buf1)-(_tcslen(buf1)*sizeof(TCHAR)));
				udpLog(buf1);
			}
			if( LogArh[SYS_RPC].arh )
				PutToArchive(bufIn,LenIn,false);
		}

		// -----------------------------------------------------
		IndxPolling ++;				// след. БМ, если не было ТУ
		// -----------------------------------------------------

	}
}



//04.03.2011 Изменил. Вариант без отдельной ф-ии ожд маркера
/*
BOOL CRpc::GetDataFromLine()
{
	rpcLastError = RpcDlgPacker::ERRTOUT;			// по умолчанию ошибка тайи-аута
	memset (bufIn,0,sizeof(bufIn));
	int indx = 0;

	DWORD tick = GetTickCount();
	while ( bufIn[indx] != RpcDlgPacker::MARKER )	// Ожидаем маркер, пропуская мусор с контролем таймаута
	{
		//01.03.2010 Если идет мусор, то подвисаем.В этом случае,выходим по таймауту.
		if ((GetTickCount()-tick) > 500 ||
			 !GetRsByte(this,&bufIn[indx]))
		{
			nErrors++;
			return FALSE;							// 
		}
		nRcvBt++;
	}
	
	if( !GetMarker() )
		return FALSE;

	indx++;
	if (!GetRsByte(this,&bufIn[indx++]))			// младший байт длины
		return FALSE;
	nRcvBt++;

	if (!GetRsByte(this,&bufIn[indx++]))			// старший байт длины
		return FALSE;
	nRcvBt++;
	
	WORD len = bufIn[1] + bufIn[2] * 256;			// длина = [1] + 256*[2]
	if (!RpcDlgPacker::CheckAnyLen(len))
	{
		nErrors++;
		rpcLastError = RpcDlgPacker::ERRLEN;		// ошибка длины
		return FALSE;
	}

	// Длина включает все, кроме маркера, принимаем отсавшиеся len-2 байта
	for (int i=0; i<len-2; i++)						// прием пакета
	{
		if (!GetRsByte(this,&bufIn[indx++]))		
			return FALSE;
		nRcvBt++;
	}

	// Приняли. Разбор пакета
	RpcDlgPacker * p = (RpcDlgPacker *)bufIn;
	if (!p->IsPackTsCorrect(rpcLastError))
	{
		nErrors++;
		if (m_pWnd)
			m_pWnd->SendMessage(WM_USER_RPC_ERROR,0,(LPARAM)p);
		return FALSE;
	}

	rpcLastError = 0;
	m_TimeLastRecv = CTime::GetCurrentTime().GetTime();
	
	nRcvPk ++;
	if (m_pWnd && bShowDetail)		
		m_pWnd->SendMessage(WM_USER_RPC_RECEIVE,p->GetFullLen(),(LPARAM)p);

	return TRUE;
}
*/

bool CRpc::GetRsByteRpc(CRS232* pRS,BYTE* pByte)
{
#if  defined(DEF_RPC_SIMULATE)
	bool bRet = GetRsByteRpcSimulate(pRS,pByte);
#else
	bool bRet = GetRsByte(pRS,pByte);
#endif
	if (bRet)
		nRcvBt++;
	
	return bRet;
}

//Вариант c отдельной ф-й ожд маркера
BOOL CRpc::GetDataFromLine()
{
	if( !GetMarker() )
		return FALSE;

	if (!GetRsByteRpc(this,&bufIn[LenIn++]) ||			// младший байт длины
		!GetRsByteRpc(this,&bufIn[LenIn++]) )			// старший байт длины
	{
		rpcLastError = RpcDlgPacker::ERRFRMT;			// ошибка формата
		return FALSE;
	}
	
	WORD len = bufIn[1] + bufIn[2] * 256;				// длина = [1] + 256*[2]
	if (!RpcDlgPacker::CheckAnyLen(len))
	{
		rpcLastError = RpcDlgPacker::ERRLEN;			// ошибка длины
		return FALSE;
	}

	// Длина включает все, кроме маркера, принимаем отсавшиеся len-2 байта
	for (int i=0; i<len-2; i++)							// прием пакета
	{
		if (!GetRsByteRpc(this,&bufIn[LenIn++]))		
		{
			rpcLastError = RpcDlgPacker::ERRFRMT;		// ошибка формата
			return FALSE;
		}
	}

	// Приняли. Разбор пакета
	RpcDlgPacker * p = (RpcDlgPacker *)bufIn;
	if (!p->IsPackTsCorrect(rpcLastError))
		return FALSE;

	//05.03.2011.Перенес проверку адреса БМ из ExtractTs
	// Ищем БМ по адресу в списке. Если не нашли - ошибка адреса
	int indxBM = GetIndxBm(p->addrBM & 0xFFFFFF3F);
	if (indxBM == -1)
	{
		rpcLastError = RpcDlgPacker::ERRADDR;
		return FALSE;
	}

	//rpcLastError = 0;
	//m_TimeLastRecv = CTime::GetCurrentTime().GetTime();
	//18.09.2012 Изменил, использую счетчик времени КП
	//CRpc::arTimeLast.SetAt(indxBM,CTime::GetCurrentTime().GetTime());
	CRpc::arTimeLast.SetAt(indxBM,gTimeCnt);
	if( m_bAckTout )
	{
		//04.03.2011 Незя,т.к в блоке диагностики передается диаг от Rpc
		//PutAckTu(0,ACK_MPC_RECONNECT);									// связь возобновилась
		if( !IsTout() )
		{
			AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(LPARAM)_T("РПЦ.Связь восстановлена"));
			m_bAckTout = false;
		}
	}

	nRcvPk ++;
	if (m_pWnd && bShowDetail)		
		m_pWnd->SendMessage(WM_USER_RPC_RECEIVE,p->GetFullLen(),(LPARAM)p);

	return TRUE;
}

//Ожидание маркера
bool CRpc::GetMarker()
{
	LenIn = 0;
	memset(bufIn,0,sizeof(bufIn));
	rpcLastError = 0;

	int i=0;
	DWORD tick = GetTickCount();
	while( i<sizeof(bufIn) )								// Ожидаем маркер
	{
		if( !GetRsByteRpc(this,&bufIn[i]) )
		{
			//if( !m_bTout && IsTout() )					// ошибка таймаута формируется однократно
			if( IsTout() )									// ошибка таймаута формируется всегда
			{
				LenIn = i;
				rpcLastError = RpcDlgPacker::ERRTOUT;		// ошибка таймаута
				if( !m_bAckTout )
				{
					//04.03.2011 Незя,т.к в блоке диагностики передается диаг от Rpc
					//PutAckTu(0,ACK_MPC_ERR_TOUT);
					AfxGetMainWnd()->SendMessage(WM_USER_LOG_MESSAGE,0,(LPARAM)_T("РПЦ.Связь отсутствует"));
					m_bAckTout = true;
				}
			}
			return false;
		}
		if( bufIn[i] == RpcDlgPacker::MARKER )
		{
			bufIn[0] = RpcDlgPacker::MARKER;
			LenIn = 1;
			return true;
		}
		i++;
		if( (GetTickCount()-tick) > 500 ) 					// Если идет мусор, то подвисаем.
			break;
	}

	LenIn = i;
	rpcLastError = RpcDlgPacker::ERRGBG;					// это мусор
	
	return false;

}

// Вытащить ТС и положить, куда надо
// ПРОБЛЕМА: учесть распределение ТС при наличии более одного БМ
BOOL CRpc::ExtractTs()
{
	RpcDlgPacker * p = (RpcDlgPacker *)bufIn;

	//05.03.2011.Проверка в GetDataFromLine, зачем повторять
//	if (!p->IsPackTsCorrect(rpcLastError))
//		return FALSE;
	
	//05.03.2011.Изменил,основная проверка в GetDataFromLine
//	// Ищем БМ по адресу в списке. Если не нашли - ошибка адреса
	int indxBM = GetIndxBm(p->addrBM & 0xFFFFFF3F);
	if (indxBM == -1)
//	{
//		rpcLastError = RpcDlgPacker::ERRADDR;
//		nErrors++;
//		if (m_pWnd)
//			m_pWnd->SendMessage(WM_USER_RPC_ERROR,(WPARAM)LenIn,(LPARAM)p);
		return FALSE;
//	}
	DiagStatusRpc[indxBM] = BYTE (p->addrBM & 0xC0);	// в служебный байт помещаем признак первая/вторая БМ шкафа 
														// (0x40 - первая, 0x80 - вторая)

	// Определяем смещение в массиве TsStatusRpc
	int offs = arTsOffset[indxBM] * 2;

	// Вычисляем длину с контролем размера массива TsStatusRpc
	int Len  = min ((int)sizeof (TsStatusRpc) - offs, 2 * min (p->GetTsGroups(),arTsGroups[indxBM]));
	memmove (TsStatusRpc + offs, p->GetTsPtr(),Len);

	// Записываем первую группу диагностики по обеим БМ в массив DiagStatusRpc
	memmove (&DiagStatusRpc[ourDiagLen + indxBM * RpcDlgPacker::DIAG_LEN * 2    ], p->GetDiag1Ptr() + 1, RpcDlgPacker::DIAG_LEN);
	memmove (&DiagStatusRpc[ourDiagLen + indxBM * RpcDlgPacker::DIAG_LEN * 2 + 3], p->GetDiag2Ptr() + 1, RpcDlgPacker::DIAG_LEN);

	return TRUE;
}

// Определить индекс (порядковый номер БМ) в списке БМ по адресу
// Если такого адреса нет - ошибка адреса. 
int CRpc::GetIndxBm(DWORD addr)
{
	for (int i=0; i<arAddrBmEx.GetSize(); i++)
	{
		if (arAddrBmEx[i]==addr)
			return i;
	}
	return -1;
}


// форматирование команды ТУ РПЦ в виде:  0xMM    OO    TT 
// MM - номер модуля HEX   
// OO - номер выхода начиная с 1
// TT - время, сек
void CRpc::StrRpcTu (WORD tu, LPTSTR buf, int buflen)
{
	// 1. Номер выхода напрямую
	int j = rpcGetOutFromTu (tu);		

	// 2. Номер модуля в кодировке проектной таблицы РПЦ
	int m = rpcGetProektMdl(rpcGetMdlFromTu(tu));
	
	// 3. Время в сек
	int t = rpcGetDelpcFromTu(tu);

	_sntprintf(buf,buflen,_T("РПЦ.0x%.2X    %.2d    %.1f"), m,j,(float)t/10);
}

/*
//04.03.2011 Незя передавать квитанции,т.к в блоке диагностики передается диаг от Rpc
//Отправка квитанции о типе ошибки связи
void CRpc::SendAckErr(int err)
{
	if( err>=0 )
		return;

	if( err == RpcDlgPacker::ERRTOUT )
		return;
	else
	if( err == RpcDlgPacker::ERRGBG )
		PutAckTu(0,ACK_MPC_ERR_GBG);
	else
	if( err == RpcDlgPacker::ERRCRC )
		PutAckTu(0,ACK_MPC_ERR_CRC);
	else
		PutAckTu(0,ACK_MPC_ERR_FORMAT);
}
*/

//Проверка наличия связи с Rpc
bool CRpc::IsTout()
{ 
	for( int i=0; i<CRpc::arTimeLast.GetSize(); i++ )
	{
		//18.09.2012 Изменил, использую счетчик времени КП
		//if( (CTime::GetCurrentTime().GetTime() - CRpc::arTimeLast.GetAt(i)) >= 15 )
		if( (gTimeCnt - CRpc::arTimeLast.GetAt(i)) >= 15 )
			return true;
	}
	return false; 
}

/*
// вывод юникод строки
bool CRpc::udpLogRpc (TCHAR * ptr,bool bPrmPrd)
{
	if( LogArh[SYS_RPC].log && ptr ) 
	{
		memset(tBuf,0,sizeof(tBuf));
		_sntprintf(tBuf,sizeof(tBuf)/sizeof(TCHAR)-1,_T("РПЦ%s%s"),bPrmPrd ? _T("<-") : _T("->"),ptr);
		return udpLog (tBuf);
	}

	return false;
}

// вывод строки байт
bool CRpc::udpLogRpc (BYTE *ptr, int len, bool bPrmPrd)
{
	if( LogArh[SYS_RPC].log && ptr ) 
	{
		memset(tBuf,0,sizeof(tBuf));
		_snprintf((PCHAR)tBuf,sizeof(tBuf)/sizeof(TCHAR)-1,"РПЦ%s%s",bPrmPrd ? "<-" : "->",ptr);
		return udpLog ((PBYTE)tBuf,strlen((PCHAR)tBuf));
	}

	return false;
}
*/

// Вывод HEX строки
bool CRpc::udpLogHexRpc (BYTE *ptr, int len, bool bPrmPrd)
{
	if( LogArh[SYS_RPC].log && ptr ) 
	{
		memset(tBuf,0,sizeof(tBuf));
		PCHAR p = (PCHAR)tBuf;
		sprintf(p,"РПЦ%s",bPrmPrd ? "<-" : "->");
		//11.04.2011
		//MakeHexStrANSII(ptr,len,p+strlen(p),(sizeof(tBuf)/sizeof(TCHAR)-strlen(p))-1);
		MakeHexStrANSII(ptr,len,p+strlen(p),sizeof(tBuf)/sizeof(TCHAR)-strlen(p));
		return udpLog((PBYTE)tBuf,strlen((PCHAR)tBuf));
	}

	return false;
}

//Отправить в архив
//- pBuf	- данные для отправки в архив в формате ЭЦМПК(ПРМ или ПРД)
//- Len		- длина данных(в байтах)
//- bPrmPrd	- =false - ПРМ, =true - ПРД
void CRpc::PutToArchive(PBYTE pBuf,int Len,bool bPrmPrd)
{
	if(	!pBufArh || LogArh[SYS_RPC].size<sizeof(ARH_HEADER) || 
		!pBuf || !Len   )
		return;

	//Определяем протокол
	pBufArh[0] = ARH_RPC;
	pBufArh[1] = ARH_TYPE_UNDEF;								// тип пакета
	pBufArh[2] = 0;
	
	//Определяем тип пакета: ТС или ТУ
	RpcDlgPacker *p = (RpcDlgPacker *)bufIn;
	if( !bPrmPrd )												// пакет ПРМ
	{
		if( p->typ == RpcDlgPacker::TYPE_TS )
			pBufArh[1] = ARH_TYPE_TC;
	}
	else														// пакет ПРД
	{
		if( p->typ == RpcDlgPacker::TYPE_TU )
			pBufArh[1] = ARH_TYPE_TU;
	}

	//Вычисляем длину с учетом префикса
	//03.02.2011 Изменил 
//	int len = Len > (RpcDlgPacker::MAX_LEN-sizeof(ARH_HEADER)) 
//		? (RpcDlgPacker::MAX_LEN-sizeof(ARH_HEADER)) 
//		: Len;
//	memcpy(&pBufArh[sizeof(ARH_HEADER)],pBuf,len);
//	udpArh (pBufArh, len+sizeof(ARH_HEADER));					// Отправляем в архив
	int size = LogArh[SYS_RPC].size - sizeof(ARH_HEADER);
	if( Len < size )
		size = Len;
	memcpy(&pBufArh[sizeof(ARH_HEADER)],pBuf,size);
	udpArh (pBufArh, size+sizeof(ARH_HEADER));					// Отправляем в архив

}


///////////////////////////////////////////////////////////////////////////////
//--------------------- Rpc Simulate -----------------------------------------
#if  defined(DEF_RPC_SIMULATE)

bool CRpc::GetRsByteRpcSimulate(CRS232* pRS,BYTE* pByte)
{
	return false;
}

#endif
///////////////////////////////////////////////////////////////////////////////


