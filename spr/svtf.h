#ifndef SVTF_H
#define SVTF_H

#include "station.h"

class Svtf : public SprBase
{
    friend class DlgSvtfInfo;
public:

    enum SVTF_CLASS
    {
        SVTF_X      = 0,
        SVTF_IN,                                            // "ВХ"
        SVTF_OUT,                                           // "ВЫХ"
        SVTF_MRSH,                                          // "МРШ"
        SVTF_PRH,                                           // "ПРХ"
        SVTF_MNV,                                           // "МНВ"
        SVTF_PRLS,                                          // "ПРС"
    };

    // открытые функции
    Svtf(SprBase * tuts, Logger& logger);                   // конструктор по ТС/ТУ
    ~Svtf();

    // открытые статические функции
    static bool AddTemplate(class IdentityType *);          // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
    static bool AddTs       (QSqlQuery& query, class Ts * ts, Logger& logger);// добавить ТС
    static bool AddTu       (QSqlQuery& query, class Tu * tu, Logger& logger);// добавить ТС

    static Svtf * GetById(int no);                          // получить справочник по номеру светофора
    static void AcceptTS (class Station *);                 // обработка объектов по станции


    //QString& SvtfDiag() { return svtfdiag; }              // тип диагностики
    bool IsTypeIn   () { return svtftype == SVTF_IN;  }
    bool IsTypeOut  () { return svtftype == SVTF_OUT; }
    bool IsTypeMrsh () { return svtftype == SVTF_MRSH;}
    bool IsTypePrh  () { return svtftype == SVTF_PRH; }
    bool IsTypeMnv  () { return svtftype == SVTF_MNV; }
    bool IsTypePrgl () { return svtftype == SVTF_PRLS;}

    void SetRcBefore (class Rc * rc) { rcBefore = rc; }     // РЦ перед светофором
    void SetRcAfter  (class Rc * rc) { rcAfter  = rc; }     // РЦ после светофора

    bool IsOpen     () { return SafeValue(opened); }        // открыт
    bool IsBlocked  () { return SafeValue(locked); }        // заблокирован
    bool IsMnvRout  () { return SafeValue(mnvM); }          // установлен маневровый маршрут от светофора
    bool IsPzdRout  () { return SafeValue(pzdM); }          // установлен поездной маршрут от светофора
    bool IsRzbl     () { return SafeValue(seltounlock); }   // выбран для разблокирования
    bool IsAd       () { return SafeValue(ad); }            // светофор на автодействии
    bool IsOm       () { return SafeValue(canceling); }     // идет отмена маршрута от светофора
    bool IsYel      () { return SafeValue(yelllow); }       // желтый огонь светофора (АБТЦМ)
    bool IsPulsing  () { return SafeValue(blinking); }      // мигающее показание светофора
    bool IsPrgls    () { return SafeValue(calling); }       // пригласит.огонь светофора
    bool IsMnvrEx   () { return SafeValue(manevr); }        // доп.контроль маневрового сигнала
    bool IsAlarm    () { return formula_er && formula_er->ValueBool(); }

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

    QString svtfdiag;                                       // тип диагностики
    QString svtftypename;                                   // тип: ВХ/ВЫХ/МРШ/ПРХ/МНВ/ПРС
    QString svtferror;                                      // логич.выражение - контроль аварии светофора

    SVTF_CLASS  svtftype;                                   // тип:    "ВХ","ВЫХ","МРШ","ПРХ","МНВ","ПРС"

    class BoolExpression * formula_er;                      // указатель на вычислитель формулы аварии светофора (BoolExpression)

    class Rc * rcBefore;                                    // РЦ перед светофором
    class Rc * rcAfter;                                     // РЦ после светофора

    bool stsOpen;                                           // открыт

    // закрытые функции
    void Accept();                                    // обработка ТС
    SprBase::UniStatusRc GetUniStatus();                    // получить статус UNI
};

#endif // SVTF_H
