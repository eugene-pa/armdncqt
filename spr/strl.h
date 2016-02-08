#ifndef STRL_H
#define STRL_H

#include "station.h"


class Strl : public SprBase
{
    friend class LinkedStrl;
    friend class Station;
    friend class DlgStrlInfo;

public:

    enum STRL_STATAUS                                       // положение стрелки
    {
        STRL_MINUS      = -1,
        STRL_UNDEFINED  = 0,
        STRL_PLUS       = 1,
    };

    // открытые функции
    Strl(SprBase * tuts, Logger& logger);                    // конструктор по ТС/ТУ
    ~Strl();

    void SetRc(class Rc* rc);                               // связать стрелку с РЦ

    // положение стрелки
    STRL_STATAUS status() { return stsPls == stsMns ? STRL_UNDEFINED :
                                   stsPls           ? STRL_PLUS      :
                                                      STRL_MINUS; }
    bool isPlus () { return status() == STRL_PLUS; }
    bool isMinus() { return status() == STRL_MINUS; }

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
    Property * selectedvsa_p;                               // выбор для СА+
    Property * selectedvsa_m;                               // выбор для СА-
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

    class Rc * sprRc;                                       // РЦ, в которую входит стрелка
    class Rc * sprRc2;                                      // вторая РЦ, в которую входит спаренная стрелка

    bool stsPls;                                            // +
    bool stsMns;                                            // -
    bool stsPlsPrv;                                         // + в пред.шаге
    bool stsMnsPrv;                                         // - в пред.шаге
    bool stsPlsRq;                                          // переводим в +
    bool stsMnsRq;                                          // переводим в -
    bool stsInformed;                                       // уведомляли
    bool stsMu;                                             // МУ
    bool stsZs;                                             // ЗС
    bool stsZpr;                                            // ЗПР
    bool stsOtuPlus;                                        // ВСА в +
    bool stsOtuMinus;                                       // ВСА в -
    bool stsOtu;                                            // ВСА
    bool stsVzres;                                          // взрез
    bool stsSelRsbl;                                        // выбор для разблокировки

    bool stsBusyRc;                                         // занятость РЦ под стрелкой
    bool stsBusyAnyRc;                                      // занятость любой из двух РЦ под спаренными стрелками
    bool stsZmkRc;                                          // замыкание РЦ под стрелкой
    bool stsZmkAnyRc;                                       // замыкание любой из двух РЦ под спаренными стрелками
    bool stsIrRc;                                           // ИР РЦ под стрелкой
    bool stsIrAnyRc;                                        // ИР любой из двух РЦ под спаренными стрелками


    // закрытые функции
    void Accept();                                          // обработка ТС
    SprBase::UniStatusRc GetUniStatus();                    // получить статус UNI
};

// описание стрелки в заданном положении
// Идея: для формализации описания стрелок в маршруте типа "2/4+ 6- 8+" преобразуем их в список LinkedStrl
class LinkedStrl
{
    friend class Rc;
    friend class Station;
    friend class Route;
    friend class NxtPrv;
    friend class DlgRcInfo;
    friend class ShapeRc;
public:

    LinkedStrl(int no);                                     // конструктор по номеру стрелки со знаком
    LinkedStrl(class Station* st, QString& name);           // конструктор по имени контроля и станции

    bool valid() { return strl != nullptr; }                // проверка валидности описания

    Strl::STRL_STATAUS  rqStatus() { return no > 0 ? Strl::STRL_PLUS : Strl::STRL_MINUS; } // требуемое положение

    bool isok()                                             // проверка нахождения в заданном положении
    {
        return strl == nullptr ? true : no < 0 ? strl->stsMns : strl->stsPls;
    }
    QString& Name() { return name; }                        // имя: "1/3+"

    // статическая функция проверка валидности списка связей
    static bool checkList(QVector<LinkedStrl*> list, Logger* logger)
    {
        bool ret = true;
        foreach (LinkedStrl * l, list)
        {
            if (!l->valid())
            {
                ret = false;
                if (logger!=nullptr)
                    logger->log(QString("Не найдена стрелка #").arg(l->no));
            }
        }
        return ret;
    }

    // статическая функция проверка положения стрелок списка связей
    // функция возвращает 0, если все ОК, или номер первой наайденной стрелки в неправильном положении
    static int checkRqSts(QVector<LinkedStrl*> list)
    {
        foreach (LinkedStrl * l, list)
        {
            if (!l->isok())
                return l->no;
        }
        return 0;
    }


private:
    int   no;                                                 // номер со знаком

    class Strl * strl;
    QString name;
};



#endif // STRL_H
