#ifndef SPRBASE_H
#define SPRBASE_H

#include <QString>
#include "../common/logger.h"

class SprBase                                               // базовый класс справочника
{
public:
    SprBase();
    ~SprBase();

    enum Direction
    {
        Even    = 0,
        Odd     = 1,
    };
    int     No  ()  { return no; }                          // номер объекта
    QString& Name()  { return name; }                       // имя объекта (м.б.сложным)
    int Id  ();                                             // уникальный идентификатор с учетом круга
    int IdSt() { return nost; }
    class Station * St() { return  st; }
//  class Krug * Krug() { return krug; }
    QString& StationName();                                 // имя станции
    QString& StMessage()                                    // сообщение формата "Ст.Овечка"
        { return buf = QString("Ст.%1").arg(StationName()); }

    virtual QString ToString()                              // сообщение формата "(#номер_объекта) имя_объекта"
        { return QString(" (#%1)").arg(no); }
    virtual QString About() { return "Класс Spr"; }         // коротко о классе
    virtual QString ObjectType()  { return ""; }            // тип объекта (РЦ/СТРЕЛКА/СВЕТОФОР)

protected:
    int     no;                                             // числовой номер/идентификаторр/ключ объекта
    int     nost;                                           // номер станции
    QString name;                                           // имя объекта
    class Station * st;                                     // указатель на класс станции
    class KrugSpr * krug;                                   // указатель на класс круга

    static QString buf;                                     // статический буфер для формирования сообщений

};



#endif // SPRBASE_H
