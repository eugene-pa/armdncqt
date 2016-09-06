#ifndef PEREGON_H
#define PEREGON_H

#include "station.h"

class Peregon : public SprBase
{
public:

    // открытые статические члены
    static std::unordered_map<int, Peregon*> Peregons;                  // хэш-таблица указателей на справочники перегонов

    // открытые статические функции
    static bool ReadBd (QString&, class KrugInfo*, Logger&);            // чтение БД
    static Peregon * GetById (int no, class KrugInfo* krug = nullptr);  // получить справочник по номеру перегона

    static void AcceptTS (class Station *);                 // обработка объектов по станции

    Peregon(QSqlQuery&, class KrugInfo* , Logger&);
    ~Peregon();

private:
    int nostup;
    int nostdown;
    class Station * stup;                                   // станция сверху/слева
    class Station * stdown;                                 // станция снизу/справа
    int pathes;                                             // число путей
    bool blind;                                             // слепой
    int blindOddTime;                                       // миним.время хода по слепому перегону в нечет.направлении
    int blindEvnTime;                                       // миним.время хода по слепому перегону в чет.направлении
    bool moveOdd;                                           // перенос нечетного на другой перегон
    bool moveEvn;                                           // перенос   четного на другой перегон
    QString shortname;

    // === Контроль соответствия зависимостей ===
    time_t	timeChangeDir;                                  // Время последнего разворота перегона
    time_t	timeErBusy;                                     // Засечка ошибки занятости
    bool	bErBusyInformed;                                // уведомление ошибки занятости
    time_t	timeErFree;                                     // Засечка ошибки свободности
    bool	bErFreeInformed;                                // уведомление ошибки направлений при свободном перегоне
    time_t	timeErCompare;                                  // Засечка несоответствия занятости и направлений
    bool	bErCompareformed;                               // уведомление несоответствия занятости и направлений

    int		sKzpOdd;							            // контроль занятости перегона в неч.направлении
    int		sKzpEvn;							            // контроль занятости перегона в чет.направлении
    int		sDir1;								            // направление по 1-й станции
    int		sDir2;								            // направление по 2-й станции
    QString	dir1Odd;							            // Выражение нечетного направления по ТС левой  станции
    QString	dir2Odd;							            // Выражение нечетного направления по ТС правой станции
    int		DirActual;							            // Актуальное направление на перегоне -1/0/1
    // === Контроль соответствия зависимостей ===

    bool	chdkOn;                                         // вкл/окл контроль поездов по ЧДК


    bool	LeftOddOrient;                                  // Нестандартная ориентация изображения станции (слева - нечетные поезда)
                                                            // Поле [Station][ORIENT] = "ЧН/НЧ"
    // здесь храним номера РЦ - стыков со слепым перегоном
    // для однопутки NoRcEvnFrom = NoRcOddTo, NoRcOddFrom = NoRcEvnTo
    int    noRcEvnFrom;                                     // РЦ стык выхода на слепой  перегон  в четном направлении
    int    noRcEvnTo;                                       // РЦ стык выхода со слепого перегона в четном направлении
    int    noRcOddFrom;                                     // РЦ стык выхода на слепой  перегон  в нечетном направлении
    int    noRcOddTo;                                       // РЦ стык выхода со слепого перегона в нечетном направлении

    std::queue<class Train *> evnTrains;                    // список поездов в четном направлении
    std::queue<class Train *> oddTrains;                    // список поездов в нечетном направлении

};

#endif // PEREGON_H
