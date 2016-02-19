#ifndef PALITRA_H
#define PALITRA_H

#include "../common/defines.h"

// класс описателя отдельного состояния транспаранта
// (для состояний ON, OFF, EXT, EXT2)
class Palitra
{
public:
    Palitra()
    {
        QString family = "Segoe UI";
#ifdef Q_OS_WIN
        int size   = 11;
        int weight = 65;
#endif
#ifdef Q_OS_MAC
        family = "Segoe UI";
        int size   = 16;
        int weight = 50;
#endif
#ifdef Q_OS_LINUX
        int size   = 12;
        int weight = 50;
#endif
        font = QFont(family, size, weight);

        colorFore = colorBack = Qt::darkGray;              // по умолчанию - серая палитра
        brushFore = brushBack = Qt::lightGray;
        penFore   = penBack   = QPen(Qt::gray);
        suited = false;
        valid  = false;
    }

    QFont   font;                                           // шрифт
    QColor  colorFore;                                      // цвет переднего плана
    QColor  colorBack;                                      // цвет фона
    QBrush  brushFore;                                      // кисть переднего плана
    QBrush  brushBack;                                      // кисть фона
    QPen    penFore;                                        // перо переднего плана
    QPen    penBack;                                        // перо фона
    QString text;                                           // текст
    bool    valid;
    bool    suited;                                         // размер подогнан!

    // инициализация
    void init (QColor fore, QColor back)
    {
        valid = fore.red() | fore.green() | fore.blue() | back.red() | back.green() | back.blue();
        colorFore = fore;
        colorBack = back;
        brushFore = QBrush(fore);
        brushBack = QBrush(back);
        penFore   = QPen(fore);
        penBack   = QPen(back);
    }
};


#endif // PALITRA_H

