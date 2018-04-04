#ifndef STATIONBASE_H
#define STATIONBASE_H

#include <QString>

// облегченный класс-описатель станции для приложений, где нужна информация о станциях,
// но не нужна полная информация об объекте
// при использовании основного класса stattion класс StationBase не нужен
// конфликта не возникает, так как класс StationBase не используется внутри реализации Blackbox
// (там используется только номер станции, который и записывается в SQL)
// при необходимости, обертка функции записи сообщения, использующая класс справочника как параметр,
// может быть создана на верхнем уровне; либо, можно использровать условную трансляцию кода такой функции

class StationBase
{
public:
    // открытые статические члены
    static std::unordered_map<int, StationBase*> Stations;  // хэш-таблица указателей на справочники станций, индексированная по ключу "номер станции"
    static std::vector<StationBase*> StationsOrg;           // массив станций в порядке чтения из БД

    // открытые статические функции
    static StationBase * GetById(int no, int krug = 0);     // получить справочник по номеру станции [и круга]
    static StationBase * GetByName(QString stname);         // получить справочник по номеру станции
    static bool ReadBd (QString& conn, int krug, Logger&);  // чтение БД

    // открытые функции
    StationBase(QSqlQuery&, int krug, Logger& );            // конструктор на базе записи в БД
    ~StationBase();

    int  No     () { return no; }                           // #
    QString& Name(){ return name; }                         // имя
    int  Ras    () { return ras; }                          // #RAS
    int  Addr   () { return addr;}                          // линейный адрес

    // закрытые члены
private:
    int     krug;                                           // номер круга
    int     no;                                             // номер станции
    QString noext;                                          // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]
    QString name;                                           // имя станции
    int     ras;                                            // номер станции связи
    int     addr;                                           // линейный адрес
    int     krugId;                                         // номер круга из БД
    int     version;                                        // версия конфигурации станции (KpVersion)
    int     otuAddr;                                        // адрес в подсистеме ОТУ (СПОК/БРОК)
    QString config;                                         // строка конфигурации
    int     kpIdBase;                                       // ID КП, явно заданный в БД или 0
    int     esr;                                            // код ЕСР
    int     gidUralId;                                      // идентификация в ГИД УРАЛ в локальной НСИ
    int     gidUralIdRemote;                                // идентификация в ГИД УРАЛ, полученная из удаленного АРМ
    bool    enable;                                         // OnOff
    bool    du;                                             // станция ДУ
    int     bitmapIdRss;                                    //
    int     radioIdRss;                                     //
    int     radioIdMnt;                                     //
    int     T[4];                                           // коэффициенты связи
    QString orient;                                         // ориентация
    QString typeEC;                                         // тип ЭЦ

};

#endif // STATIONBASE_H
