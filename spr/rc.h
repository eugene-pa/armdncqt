#ifndef RC_H
#define RC_H

#include <QDateTime>

class Rc : public SprBase
{
public:
    enum RcTypes
    {
        RcFirst       = 1,
        RcLast        = 2,
        RcLeft        = 3,
        RcRight       = 4,
        RcOneToOne    = 5,
        RcOnToMany    = 6,
        RcManyToOne   = 7,
        RcManyToMany  = 8,
    };

// открытве статические функции
// открытве функции
    Rc();
    ~Rc();

    bool StsBusy () { return stsBusy; }                     // занятость

private:
    // Динамические состояния. Должны быть вычислены при получении данных
    bool stsBusy     ;                                      // занятость
    bool stsBusyFalse;                                      // ложная занятость
    bool stsBusyPrv  ;                                      // занятость в пред.цикле
    bool stsZmk      ;                                      // замыкание
    bool stsIr       ;                                      // разделка
    bool stsMu       ;                                      // местное управление
    bool stsBlock    ;                                      // блокироана РЦ

    bool stsBusyPulse;                                      // РЕТАЙМ. мигает занятость     - ???
    bool stsZmkPulse ;                                      // РЕТАЙМ. мигает замыкание     - ???

    bool stsRouteRq;                                        // в устанавливаемом маршруте   - м.б.не нужно, т.к.есть actualRoute
    bool stsRouteOk;                                        // в установленноммаршруте      - м.б.не нужно, т.к.есть actualRoute
    bool stsPassed;                                         // пройдена в маршруте          - м.б.не нужно, т.к.есть actualRoute

    class Route * actualRoute;                              // актуаьный маршрут
    class Train * actualtrain;                              // актуаьный поезд

     QDateTime tmdt;                                        // засечка последнего обновления
};

#endif // RC_H
