#ifndef OTU_H
#define OTU_H

#include "station.h"

class Otu : public SprBase
{
    friend class DlgOtu;

public:
    // открытве статические функции
    static bool ReadBd (QString& dbpath, class KrugInfo* krug, Logger& logger);       // чтение БД

    // открытве функции
    Otu(QSqlQuery& query, class KrugInfo* krug, Logger& logger);
    ~Otu();

    bool isUpok() { return upok; }
    QString& ShortName() { return shortName; }
    QString& TypeOfObject() { return objectType; }
    QString& NameTs1() { return nameTs1; }
    QString& NameTs2() { return nameTs2; }
    QString& EpilogTu(){ return epilogTu; }
    QVector <class Tu*>& EpilogTuList() { return epilogTuList; }
    class Station * St2() { return st2; }

private:
    bool upok;                                              // команда реализуется встроенной системой ОТУ УПОК+БРОК
    QString shortName;                                      // имя ОТУ
    QString objectType;                                     // имя типа ЭЦ, по алгоритму которой реализуется ОТУ или ""

    QString nameTs1;                                        // имя ТС контроля восприятия предварительной ТУ
    QString nameTs2;                                        // имя ТС контроля восприятия исполнительной ТУ

    // Дополнительные команды ТУ. Например, разблокировка перегонов и участков удаления выполняется одной групповой командой ПГРС
    // с последующим выбором объекта. При описании команды  ПГРС в этом поле перечисляются через пробел возможные варианты команд разблокировки,
    // например ЧР НР ЧРУ НРУ для формирования меню поездному диспетчеру.
    QString epilogTu;                                       // перечисление доп.ТУ
    QVector <class Tu*> epilogTuList;                       // список справочников доп.ТУ

    // НЕ РЕАЛИЗОВАНО: в варианте стыковки с РПЦ Диалог в поле Apendix можно описать команды ТУ, сопровождающие выдачу ОТУ. Запись производится через пробел в формате: ОТУ1=ИМЯ1 [ОТУ2=ИМЯ2].

    int nost2;                                              // номер станции сопряженной команды или 0
    class Station * st2;                                    // справочник станции сопряженной команды или null
    int no2;                                                // номер сопряженной команды или 0

};

#endif // OTU_H
