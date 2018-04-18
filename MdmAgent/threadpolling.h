#ifndef THREADPOLLING_H
#define THREADPOLLING_H
#include "../common/common.h"

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
const WORD DST_OFFS			= 3;
const WORD SRC_OFFS			= 4;
const WORD SEANS_OFFS		= 5;
const WORD FRAME_DATA_OFFS	= 6;

const int  LEN_CTRL_INFO    = 6;								// длина обрамления (маркер, длина, CRC, EOT)
const int  LEN_HEADER       = 3;								// длина заголовка (маркер+поле длины)
const int  CRC_SIZE         = 2;								//
*/
const BYTE SOH              = 1;
const BYTE EOT              = 4;
const BYTE CpuAddress       = 0;								// адрес ЦПУ на линии


class Station * NextSt();                                       // получить след.станцию для опроса
class Station * actualSt;                                       // актуальная станция

class RasHeader
{
public:
static BYTE counter;                                            // циклический счетчик сеансов
static int   indxSt;                                            // индекс актуальной станции опроса
    RasHeader(class Station * st);

    BYTE    marker;                                             // маркер
    WORD    length;                                             // длина пакета (все после себя, исключая CRC и EOT)
    BYTE    dst;                                                // адрес назначения
    BYTE    src;                                                // адрес источника
    BYTE    seans;                                              // сеанс
    BYTE    data[MAX_LINE_DATA_LEN + 2 + 1];                    // блоки данных, CRC + EOT

};
#pragma pack()

#endif // THREADPOLLING_H
