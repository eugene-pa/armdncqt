#ifndef STRL_H
#define STRL_H

#include "station.h"

// описание стрелки в заданном положении
// Идея: для формализации описания стрелок в маршруте типа "2/4+ 6- 8+" преобразуем их в список LinkedStrl
class LinkedStrl
{
    friend class Rc;
    friend class Station;
    friend class NxtPrv;
    friend class DlgRcInfo;
public:
    LinkedStrl(int no);

    bool IsOk()                                             // проверка нахождения в заданном положении
    {
        return true;
    }
    QString& Name() { return name; }
private:
    int no;                                                 // номер со знаком
    class Strl * strl;
    QString name;
};


class Strl : public SprBase
{
    friend class LinkedStrl;
public:

    // открытые функции
    Strl(SprBase * tuts, Logger& logger);                    // конструктор по ТС/ТУ
    ~Strl();

    // открытые статические функции
    static bool AddTemplate(class IdentityType *);          // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
    static bool AddTs       (QSqlQuery& query, class Ts * ts, Logger& logger);// добавить ТС
    static bool AddTu       (QSqlQuery& query, class Tu * tu, Logger& logger);// добавить ТС

    static Strl * GetById(int no);                          // получить справочник по номеру стрелки
    static void AcceptTS (class Station *);                 // обработка объектов по станции

private:
    static QHash <int, Strl *> strlhash;                    // СТРЛ , индексированные по индексу ТС
    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств СТРЛ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов СТРЛ

    // свойства

    Property * vzrez;                                       // взрез
    Property * selectedunlock;                              // выбор для разблокировки
    Property * selectedvsa;                                 // выбор для СА
    Property * selectedvsa_p;                               // выбор для СА-
    Property * selectedvsa_m;                               // выбор для СА+
    Property * locked;                                      // запирание
    Property * minus;                                       // контроль минус
    Property * plus;                                        // контроль плюс
    Property * mu;                                          // местное управление

    // методы
    Method   * selectvsa;                                   // выбор для СА

    Method   * selectvsa_p;                                 // выбор для СА-
    Method   * selectvsa_m;                                 // выбор для СА+
    Method   * lock;                                        // запирание
    Method   * unlock;                                      // отпирание
    Method   * setplus;                                     // перевод в минус
    Method   * setminus;                                    // перевод в плюс

    class BoolExpression * formula_zs;                      // указатель на вычислитель замыкания стрелки (BoolExpression)
    class BoolExpression * formula_mu;                      // указатель на вычислитель формулы местного управления стрелкрой (BoolExpression)ккк

                                                            // свойство name содержит имя стрелки с ознаком ±, например: 1/3±
    QString basename;                                       // свойство basename содержит только имя, например: 1/3

    // закрытые функции

};

#endif // STRL_H
