#ifndef SPRBASE_H
#define SPRBASE_H

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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

    bool Enabled() { return enabled; }                      // включен (не отключен)
    void Enable(bool s = true) { enabled = s; }             // включить

protected:
    int     no;                                             // числовой номер/идентификаторр/ключ объекта
    int     nost;                                           // номер станции
    QString name;                                           // имя объекта
    class Station * st;                                     // указатель на класс станции
    class KrugSpr * krug;                                   // указатель на класс круга
    bool    enabled;                                        // включен (не отключен)
    static QString buf;                                     // статический буфер для формирования сообщений

};

// класс описания шаблонов свойств
class IdentityType
{
public:
    IdentityType (QRegularExpression& regex, QString name); // конструктор
    QString& Name() { return name; }
    QRegularExpression Regex() { return regex; }
private:
    QRegularExpression regex;                               // шаблон
    QString name;                                           // имя свойства
};

// класс описания свойств
class Property
{
public:
    Property (IdentityType& type)
    {
        this->type = &type;
        ts = nullptr;
    }
    bool Valid() { return ts != nullptr; }                  // валидность свойства
    bool Sts();
    QString& NameTs  ();                                    // имя ТС
    QString& NameTsEx();                                    // Ст.имя_станции ТС=имя_тс
    QString& NameProp() { return type->Name(); }
    bool Parse (class Ts * ts, Logger& logger);             // разбор ТС на принадлежность свойству
    class Ts * Ts() { return ts; }                          // указатель на класс ТС свойства или NULL

private:
static QString empty;

    IdentityType * type;                                    //
    class Ts * ts;                                          // ТС, определяющий свойство
};



#endif // SPRBASE_H
