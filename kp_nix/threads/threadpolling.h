#ifndef THREADPOLLING_H
#define THREADPOLLING_H
#include "../common/common.h"

const WORD LENGTH_OFFS		= 1;
const WORD DST_OFFS			= 3;
const WORD SRC_OFFS			= 4;
const WORD SEANS_OFFS		= 5;
const WORD FRAME_DATA_OFFS	= 6;

const int  LEN_CTRL_INFO    = 6;								// длина обрамления (маркер, длина, CRC, EOT)
const int  LEN_HEADER       = 3;								// длина заголовка (маркер+поле длины)
const int  CRC_SIZE         = 2;								//
const BYTE SOH              = 1;
const BYTE EOT              = 4;
const BYTE CpuAddress       = 0;								// адрес ЦПУ на линии

#endif // THREADPOLLING_H
