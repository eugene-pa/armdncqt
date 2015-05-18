#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "colorscheme.h"

ColorScheme::ColorScheme(QString dbPath, Logger * plogger)
{
    logger = plogger;

    schemes.append(&scheme1);
    schemes.append(&scheme2);
    schemes.append(&scheme3);

    ReadBd(dbPath);
}

ColorScheme::~ColorScheme()
{

}

// статическая функция чтения цветовой схемы
bool ColorScheme::ReadBd(QString& dbPath)
{
    log(QString("Чтение цветовых схем ColorScheme: %1").arg(dbPath));

    QString sql("SELECT * FROM [ColorScheme] ORDER BY [ColorName]");

    try
    {
        QSqlDatabase dbSql = QSqlDatabase::addDatabase("QSQLITE", "qsqlite");
        dbSql.setDatabaseName(dbPath);
        if (dbSql.open())
        {
            QSqlQuery query(dbSql);
            if (query.exec (sql))
            {
                while (query.next())
                {
                    // читаем имя параметра и RGBA  цвет для всех цветовых схем
                    QString name = query.value("ColorName").toString();
                    for (int i = 0; i < schemes.count(); i++)
                    {
                        int a = query.value(QString("A%1").arg(i+1)).toInt();
                        int r = query.value(QString("R%1").arg(i+1)).toInt();
                        int g = query.value(QString("G%1").arg(i+1)).toInt();
                        int b = query.value(QString("B%1").arg(i+1)).toInt();

                        QColor clr = QColor::fromRgb(r, g, b, a);
                        schemes[i]->insert(name, clr);
                    }
                }
            }
            else
            {
                log("Ошибка выполнения запроса: " + sql);
            }
        }
        else
        {
            log("Ошибка открытия БД " + dbPath);
        }
    }
    catch(...)
    {
        log("Исключение в функции ColorScheme::ReadBd");
        return false;
    }

    // инициировать все статические инструменты рисования в соответстии с актуальной схемой
    //Shape.InitInstruments();
    return true;
}
