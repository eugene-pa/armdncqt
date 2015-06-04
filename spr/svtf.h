#ifndef SVTF_H
#define SVTF_H

#include "../common/logger.h"
#include "sprbase.h"
#include "properties.h"

class Svtf : public SprBase
{
public:
    // открытые функции
    Svtf(SprBase * tuts, Logger& logger);                   // конструктор по ТС/ТУ
    ~Svtf();

    // открытые статические функции
    static bool AddTemplate(class IdentityType *);          // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
    static bool AddTs       (class Ts * ts, Logger& logger);// добавить ТС
    static bool AddTu       (class Tu * tu, Logger& logger);// добавить ТС

private:
    static QHash <int, Svtf *> svtfhash;                     // СВТФ , индексированные по индексу ТС
    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств СВТФ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов СВТФ

    // свойства
    Property * opened;                                      // открыт
    Property * manevr;                                      // маневровое показание
    Property * calling;                                     // пригласительный
    Property * pzdM;                                        // акивен маневровый маршрут
    Property * mnvM;                                        // активен поездной маршрут
    Property * locked;                                      // блокировка
    Property * ad;                                          // автодействие
    Property * seltounlock;                                 // выбор для разблокировки
    Property * yelllow;                                     // желтый разрешающий
    Property * ko;                                          // контроль красного огня
    Property * blinking;                                    // мигание
    Property * canceling;                                   // отмена маршрута

    // методы
    Method   * open;                                        // открыть
    Method   * close;                                       // закрыть
    Method   * lock;                                        // блокировка
    Method   * unlock;                                      // разблокировка
    Method   * adon;                                        // включить автодействие
    Method   * adoff;                                       // отключить автодействие
    Method   * mm;                                          // маршрутная кнопка
    Method   * cancel;                                      // отмена маршрута

    // закрытые функции

};

#endif // SVTF_H
