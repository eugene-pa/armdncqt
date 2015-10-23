#ifndef RC_H
#define RC_H

#include "station.h"

// Класс описания дерева связей
class NxtPrv
{
    friend class Rc;
public:
    // нужно додумать конструктор
    NxtPrv (Rc * l, Rc * r, class Svtf *s1, class Svtf *s2, class Svtf *s3, class Svtf *s4)
    {
        lft = l;
        rht = r;
        svtf_LR     = s1;
        Svtf_LR_M   = s2;
        svtf_RL     = s3;
        Svtf_RL_M   = s4;
    }
    Rc *    lft;                                            // РЦ слева
    Rc *    rht;                                            // РЦ справа
    QVector	<class LinkedStrl*>strl;                        // Опред.стрелки со знаком
    class Svtf * svtf_LR;                                   // разделяющий поездной светофор   слева - направо
    class Svtf * Svtf_LR_M;                                 // разделяющий маневровый светофор слева - направо
    class Svtf * svtf_RL;                                   // разделяющий поездной светофор   справа - налево
    class Svtf * Svtf_RL_M;                                 // разделяющий маневровый светофор справа - налево
};

class Rc : public SprBase
{
    friend class Ts;
    friend class DStDataFromMonitor;                        // для формирования и извлечения информации в потоке ТС
    friend class DDataFromMonitor;
    friend class DlgRcInfo;

public:
    enum RcTypes
    {
        RcFirst       = 1,                                  // РЦ пеpвaя на участке    (самая крайняя левая )
        RcLast        = 2,                                  // РЦ последняя на участке (самая крайняя правая)
        RcLeft        = 3,                                  // РЦ крайняя левая  в станции(прием / отпр с/на перегон)
        RcRight       = 4,                                  // РЦ крайняя правая в станции(прием / отпр с/на перегон)
        RcOneToOne    = 5,                                  // одна пpедыдующая,одна следующая
        RcOnToMany    = 6,                                  // одна пpедыдующая,много следующих
        RcManyToOne   = 7,                                  // много пpедыдующих,одна следующая
        RcManyToMany  = 8,                                  // много пpедыдующих,много следующих
    };

    // открытые статические функции
    static Rc * GetById(int no);                            // получить справочник по номеру РЦ
    static Rc * GetSprByOrgNoAndKrug(int no, int bridgeno);
    static QHash<int, Rc *>& AllRc() { return rchash; }     // получить таблицу всех РЦ

    static bool ReadRelations(QString& dbpath, Logger& );   // чтение таблицы связей РЦ и формирование дерева связей

    // открытые функции
    Rc(SprBase * tuts, Logger& logger);                     // конструктор по ТС/ТУ
    ~Rc();

    bool StsBusy () { return stsBusy; }                     // занятость

    // открытые статические функции
    static bool AddTemplate (class IdentityType *);         // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
    static Rc * AddTs       (QSqlQuery& query, class Ts * ts, Logger& logger);// обработать ТС, помеченный как РЦ
    static Rc * AddTu       (QSqlQuery& query, class Tu * tu, Logger& logger);// обработать ТУ, помеченный как РЦ

    QString& Name() { return name; }                        // имя РЦ

    class Route * ActualRoute() { return actualRoute; }     // актуаьный маршрут
    class Train * Actualtrain() { return actualtrain; }     // актуаьный поезд

    QVector <Ts *> Allts() { return allts; }          // список действительных ТС объекта
    QVector <Tu *> Alltu() { return alltu; }          // список действительных ТУ объекта

private:
    static QHash <int, Rc *> rchash;                        // РЦ полигона, индексированные по ID
    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств РЦ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов РЦ

    // каждая РЦ имеет указатели на все возможные свойства РЦ; если свойство не задействовано, указатель содержит nullptr
    // хочу иметь коллекцию определенных для данной РЦ свойств и методов
    QVector <Ts *> allts;                                   // список действительных ТС объекта
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
    QVector <Tu *> alltu;                                   // список действительных ТУ объекта
    Method   * tulock;                                      // заблокировать
    Method   * tuunlock;                                    // разблокировать
    Method   * tuir;                                        // разделка

    bool	bRcHasStrl;                                     // Признак стрелочного блок-участка

    QVector<NxtPrv*> prv;                                   // список всех предыдующих РЦ с определяющими стрелками
    QVector<NxtPrv*> nxt;                                   // список всех следующих РЦ с определяющими стрелками
    NxtPrv *prvActual;                                      // активная связь влево  по стрелкам
    NxtPrv *nxtActual;                                      // активная связь вправо по стрелкам

    QVector <class DShape*> shapes;                         // динамически создаваемый список примитивов, отображающих эту РЦ; нужен для обработки стыков

                                                            // список создается для актальной видеоформы при ее открытии
    RcTypes rcTopoType;                                     // тип по топологии
    int pathno;                                             // № пути
    int pregonno;                                           // № перегона
    class Peregon * peregon;                                // перегон
    float distance;                                         // километраж
    bool  breaked;                                          // стык слепого перегона
    int   dir;                                              // -1/0/1 ODD:-1  EVN:1  ANY 0
    bool  tpoint;                                           // признак того, что это перегонная точка (имя сигнала начинается с "Т")

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

    QDateTime tmdt;                                         // засечка последнего обновления


    // закрытые функции
};

#endif // RC_H
