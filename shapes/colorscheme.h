#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QHash>
#include <QColor>
#include <QVector>
#include "../common/logger.h"

// цвета включая прозрачность
class ColorScheme
{
private:
    QHash <QString, QColor> scheme1;                        // стандартная ДЦ
    QHash <QString, QColor> scheme2;                        // СТО 1.19.005-2008 (по умолчанию)
    QHash <QString, QColor> scheme3;                        // пользовательская

    QHash <QString, QColor> * actualScheme;                 // актуальная схема
    QVector <QHash<QString, QColor> * > schemes;            // список схем
    Logger * logger;                                        // логгер

public:
    ColorScheme(QString dbPath, Logger * logger);
    ~ColorScheme();

    // типы цветовых схем
    enum ColorSchemes
    {
        Standard= 0,                                        // стандартная ДЦ
        STO     = 1,                                        // СТО 1.19.005-2008 (по умолчанию)
        User    = 2,                                        // пользовательская
    };

    // получить указанный цвет указанной типом цветовой схемы
    QColor GetColor (ColorSchemes colorScheme, QString property)
    {
        return GetColor(*schemes[(int) colorScheme], property);
    }

    // получить указанный цвет актуальной цветовой схемы
    QColor GetColor(QString property)
    {
        return GetColor (actualScheme != 0 ? *actualScheme : scheme2, property);
    }

    // получить указанный цвет указанной цветовой схемы
    QColor GetColor( QHash <QString, QColor>& scheme, QString property)
    {
        if (scheme.contains(property))
            return scheme[property];
        return Qt::black;
    }

    QColor DimColor (QColor clr)
    {
        return QColor::fromRgb(clr.red()/2, clr.green()/2, clr.blue()/2, clr.alpha());
    }

    bool ReadBd (QString& dbPath);

    void log (QString msg)
    {
        if (logger)
            logger->log(msg);
    }
};


#endif // COLORSCHEME_H
