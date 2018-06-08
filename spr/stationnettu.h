#ifndef STATIONNETTU_H
#define STATIONNETTU_H

#include "../common/defines.h"

class StationNetTU
{
public:
    StationNetTU();
//private:
    WORD	signature;							// сигнатура (55aa)									00
    WORD	length;								// полная длина сетевого блока включая сигнатуру	02
    WORD    nost;                               // номер станции
    WORD    datalength;                         // длина собственно данных
    BYTE    data[MAX_LINE_DATA_LEN];            // данные
};

#endif // STATIONNETTU_H
