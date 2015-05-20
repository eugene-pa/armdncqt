#include "sprbase.h"
#include "station.h"

QString SprBase::buf;                                       // статический буфер для формирования сообщений

SprBase::SprBase()
{
    no   = 0;                                               // числовой номер/идентификаторр/ключ объекта
    nost = 0;                                               // номер станции
    st   = nullptr;                                         // указатель на класс станции
    krug = nullptr;                                         // указатель на класс круга
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


