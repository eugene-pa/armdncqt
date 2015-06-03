#ifndef RC_H
#define RC_H

#include "../common/logger.h"
#include "sprbase.h"
#include "properties.h"


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

// открытые статические функции
    static bool AddTemplate (class IdentityType *);         // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
    static bool AddTs       (class Ts * ts, Logger& logger);// обработать ТС, помеченный как РЦ
    static bool AddTu       (class Tu * tu, Logger& logger);// обработать ТУ, помеченный как РЦ

// открытые функции
    Rc(Ts * ts, Logger& logger);                            // конструктор по ТС
    Rc(Tu * tu, Logger& logger);                            // конструктор по ТУ
    ~Rc();
    void InitProperties(Logger& logger);                    // инициализация свойств
    bool StsBusy () { return stsBusy; }                     // занятость

private:
    static QHash <int, Rc *> rchash;                        // РЦ , индексированные по индексу ТС
    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств РЦ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов РЦ

    // свойства
    Property *locked;                                       // блокировка
    Property *unlocking;                                    // восприятие разблокировки
    Property *selected_ir;                                  // выбор для разделки
    Property *zmk;                                          // замыкание
    Property *busy;                                         // зканятость
    Property *ir;                                           // ИР
    Property *falsebusy;                                    // ложная занятость
    Property *mu;                                           // МУ
    Property *uri;                                          // разделка в терминах ЭЦ/ЭЦ МПК

    // методы
    Method   * tulock;                                      // заблокировать
    Method   * tuunlock;                                    // разблокировать
    Method   * tuir;                                        // разделка


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
