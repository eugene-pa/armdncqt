#ifndef RASPACKER_H
#define RASPACKER_H

#include <../common/defines.h>

#pragma pack(1)

// Формат пакетов вниз/вверх симметричен
//								  FRAME_DATA_OFFS
//								  |			   DATA_OFFS
//								  |			   |
//               0 1 2    3 4 5   6 7 8 9 1011   12 13 14           CRC  EOT
//              |-|-|-|  |-|-|-|  -|-|-|-|-|-|  |--|--|--|--|--|
// HEADER -------H LLL    | | |   | | | | | |    ---------------- Buf_Tu[]
// length ---------+++	  | | |   | | | | | +---- резерв (11 байт)
// (все после себя,       | | |   | | | | +------ длина блока управления диагностич.системой
//	кроме CRC)			  | | |   | | | +-------- длина блока информации СПОК
//                        | | |   | | +---------- длина блока команд ТУ/ТС (0 - n-1)
//                        | | |   | +------------ длина блока системной информации (0 - n-1)
//                        | | |   +-------------- расширение длин инфо блоко
//                        | | +----------------- -сеанс
//						  | +-------------------- адрес источника
//                        +---------------------- адpес ЛПУ назначения

/*
const WORD LENGTH_OFFS		= 1;
const WORD SEANS_OFFS		= 5;
const WORD FRAME_DATA_OFFS	= 6;


const int  CRC_SIZE         = 2;								//
*/
const int  LEN_HEADER       = 3;								// длина заголовка (маркер+поле длины)
const int  LEN_CTRL_INFO    = 6;								// длина обрамления (маркер, длина, CRC, EOT)
const int  DST_OFFS			= 3;                                // смещение адреса назначения в пакете
const int  SRC_OFFS			= 4;                                // смещение адреса источника  в пакете
const BYTE SOH              = 1;
const BYTE EOT              = 4;
const BYTE CpuAddress       = 0;								// адрес ЦПУ на линии

const BYTE MSK_LEN_EXT      = 3;                                // маска расширения длины
const BYTE SYSBLCK          = 0;                                // мещения блоков
const BYTE TUTSBLCK         = 1;
const BYTE OTUBLCK          = 2;
const BYTE DIAGBLCK         = 3;

const WORD	RQ_WATCHDOG_ON		= 0x000e;                       // включение сторожевого таймера
const WORD	RQ_RESTART_COUNT	= 0x000f;                       // запрос количества перезапусков

// подготовленные готовые пакеты для директив SYSINFO
//									+----------------------------------------- расширение длины
//									|     +----------------------------------- длина сисинфо
//									|	  |	    |+---------------------------- длина ТУ
//									|	  |		|	  +----------------------- длина О
//									|	  |		|	  |     +----------------- длина Д
//									|	  |		|	  |		|     +----------- резерв
//									|	  |		|	  |		|     |     +----- команда
//									|	  |		|	  |		|     |     |
const BYTE TuPackRestart     [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff };
const BYTE TuPackSetMain     [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 };
const BYTE TuPackSetRsrv     [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00 };
const BYTE TuPackBypassMain  [10] = { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 ,0x00, 0x00};
const BYTE TuPackBypassRsrv  [10] = { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00 ,0x00, 0x00};
const BYTE TuPackResetAtu    [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00 };
const BYTE TuPackSpeedLimit  [10] = { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00 ,0x00, 0x00};
const BYTE TuPackResetOne    [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00 };			// перезапуск себя
const BYTE TuPackResetAnother[8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00 };			// перезапуск соседа
const BYTE TuPackTestMtuMts  [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00 };			// тест ТУ/ТС
const BYTE TuPackTimeSet     [12] = { 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00 ,0x00, 0x00, 0x00, 0x00};	// синхронизация времени
const BYTE TuPackWatchdogOn  [8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, RQ_WATCHDOG_ON  , 0x00 };	// 2009.02.04. включение сторожевого таймера
const BYTE TuPackRestartCount[8 ] = { 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, RQ_RESTART_COUNT, 0x00 };	// 2011.09.14запрос количества перезапусков



class RasPacker
{
public:
static int   indxSt;                                            // индекс актуальной станции опроса
    RasPacker(class Station * st);                              // когструктор
    int Length() { return length + LEN_CTRL_INFO;  }            // общая длина пакета
    class RasData * GetRasData() { return (class RasData *)&data; }
    bool IsEmpty() { return length == 3; }                      //
//private:
    BYTE    marker;                                             // маркер
    WORD    length;                                             // длина пакета (все после себя, исключая CRC и EOT)
    BYTE    dst;                                                // адрес назначения
    BYTE    src;                                                // адрес источника
    BYTE    seans;                                              // сеанс

    // далее идут длины блоков, блоки (класс RasData), CRC и EOT
    BYTE    data[MAX_LINE_DATA_LEN + 2 + 1];                    // блоки данных, CRC + EOT
    class Station * st;
};

// класс-обертка данных станции связи
// именно эти данные передаются в модуль управление
class RasData
{
public:
    RasData()  { Clear(); }
    bool Empty() { return Length() == 0; }                                                      // пустой
    int Length     ();                                                                          // oбщая длина данных
    int Length     (int n);                                                                     // длина заданного блока
    int LengthFrom (int n);                                                                     // лина блоков, начиная с заданного
    int LengthSys () { return sysLength  + 256 * ( extLength       & 0x03); }                   // длина блока системной информации
    int LengthTuts() { return tutsLength + 256 * ((extLength >> 2) & 0x03); }                   // длина блока ТС/ТУ
    int LengthOtu () { return otuLength  + 256 * ((extLength >> 4) & 0x03); }                   // длина блока ОТУ
    int LengthDiag() { return diagLength + 256 * ((extLength >> 6) & 0x03); }                   // длина блока квитанций и диагностики

    void SetBlockLen (WORD Blck,int Len);

    QString About() { return QString("[%1.%2.%3.%4]").arg(LengthSys()).arg(LengthTuts()).arg(LengthOtu()).arg(LengthDiag()); ; }

    BYTE * PtrBlck (int n);                                     // указатель на заданный блок
    BYTE * PtrSys  () { return data; }
    BYTE * PtrTuTs () { return &data[LengthSys ()                               ]; }
    BYTE * PtrOtu  () { return &data[LengthSys () + LengthTuts()                ]; }
    BYTE * PtrDiag () { return &data[LengthSys () + LengthTuts() + LengthOtu () ]; }

    void Copy  (RasData* prc);                                  // обайтное копирование 
    void Clear ();                                              // очистка
    void Append(RasData* prc, class Station*);                  // суммирование инфо-блоков (если не успели отправить старую посылку)

private:
    void AppendBlock(RasData* pSrc, BYTE blck);                 // суммирование аданного инфо-блока
    void DeleteBlock(BYTE blck);                                // очистить информацию по блоку
    BYTE extLength;                                             // расширение длин
    BYTE sysLength;                                             // длина блока систе.информации
    BYTE tutsLength;                                            // длина блока ТУ/ТС
    BYTE otuLength;                                             // длина блока ОТУ
    BYTE diagLength;                                            // длина блока квитанций и диагностики
    BYTE reserve;                                               // резерв
    BYTE data[1023*4];                                          // блоки данных
};


#pragma pack()

#endif // RASPACKER_H
