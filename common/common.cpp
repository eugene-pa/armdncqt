#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;

// ВЫНЕСТИ В ОБЩИЙ ФАЙЛ
// 1. Побайтовый алгоритм вычисления CRC
//    (Р.Л.Хаммел,"Послед.передача данных", Стр.49. М.,Мир, 1996)
const WORD CRC_POLY = 0x1021;
WORD GetCRC (BYTE *buf,WORD Len)
{
    WORD j,w,Crc = 0;
    while (Len--)
    {
        w = (WORD)(*buf++)<<8;
        Crc ^= w;
        for (j=0; j<8; j++)
            if (Crc & 0x8000)
                Crc = (Crc<<1) ^ CRC_POLY;
            else
                Crc = Crc << 1;
    }
    return Crc;
}

// ВЫНЕСТИ В ОБЩИЙ ФАЙЛ
std::wstring GetHexW(void *data, int length)
{
    std::wstringstream tmp;

    for (int i=0; i < length; i++)
    {
        tmp << std::setfill(L'0') << std::setw(2)<< std::hex << ((BYTE*)data)[i] << " ";
    }
    return tmp.str();
}
