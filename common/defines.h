#ifndef DEFINES_H
#define DEFINES_H

#include <QGraphicsItem>
#include <QColor>
//#include <QWidget>
//#include <QGraphicsView>

#define BYTE unsigned char
#define WORD unsigned short
#define UINT unsigned int
#define SIGNATURE 0xAA55
#define DUBL 2
#define MAX_TRAINS_ON_PER 5

#define LPCTSTR const char *
#define COLORREF QColor
#define BOOL bool
#define FALSE false
#define TRUE  true
#define CString QString
#define CPen QPen
#define CBrush QBrush
#define CDC  QPainter
#define CFont QFont
#define CBitmap QBitmap
#define strstr(str,text) (str.indexOf(text)>=0)
#define strcmp(str1,str2) (str1==str2)
#define strlen(str) str.length()
class DStation;

//QGraphicsView

#endif // DEFINES_H
