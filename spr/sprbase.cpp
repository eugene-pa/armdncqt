#include "sprbase.h"
#include "station.h"
#include "ts.h"

QString SprBase::buf;                                       // статический буфер для формирования сообщений

SprBase::SprBase()
{
    no   = 0;                                               // числовой номер/идентификаторр/ключ объекта
    nost = 0;                                               // номер станции
    st   = nullptr;                                         // указатель на класс станции
    krug = nullptr;                                         // указатель на класс круга
    enabled = true;                                         // включен
}

SprBase::~SprBase()
{

}

// если определен круг, выдать ключ = (krugno<<16 | no )
int SprBase::Id  ()
{
    return no;
}

// получить имя станции если есть справочник, или строку "#номер_станции"
QString& SprBase::StationName()
{
    if (st != nullptr )
        return st->Name();
    return buf = (QString("#%1").arg(nost));
}


QString Property::empty;
bool Property::Sts()                                        // логическое состояние свойства
{
    return Valid() ? ts->Sts() : false;
}

QString& Property::NameTs()                                 // имя ТС
{
    return ts == nullptr ? empty : ts->Name();
}

QString& Property::NameTsEx()                               // Ст.имя_станции ТС=имя_тс
{
    return ts == nullptr ? empty : ts->NameEx();
}

// разбор ТС на принадлежность свойству
bool Property::Parse (class Ts * ts, Logger& logger)
{
    bool ret= false;
    if (this->ts == nullptr)
    {
        if (type->Regex().)
        //QRegularExpressionMatchIterator matches = type->regex.globalMatch(ts->Name());
        if (ma)
    }
    else
    {
        logger.log(QString("Переопределение свойства %1: %2, %3").arg(NameProp()).arg(NameTsEx()).arg(ts->NameEx()));
    }
    return ret;
}
