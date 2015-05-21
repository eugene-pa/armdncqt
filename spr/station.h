#ifndef STATION_H
#define STATION_H

#include <QObject>

#include <QHash>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QBitArray>

#include "../common/logger.h"
#include "ts.h"

class Station : public QObject
{
    Q_OBJECT

public:

    enum KpVersion
    {
        VERSION32X32            = 1,                        // Ломиконт
        VERSION_8X8             = 2,                        // КП-1999 - промежуточная, некорректный CRC
        VERSIONKp1999           = 3,                        // КП-1999
        VERSION2000             = 4,                        // КП-2000
        VERSION2000Rpctums      = 5,                        // КП-2000 + РПЦ
        VERSIONRetime           = 6,                        // Ретайм (PZUVER=1, bRssSetun=true, bRssSetun2=true)
        VERSION2007             = 7,                        // КП2004-2007
        VERSIONKp2000Lomikont   = 8,                        // Гибрид с матрицей ЛОМИКОНТА (PZUVER=1, bLomikontKP2000 = true;)
//      VERSION_EBILOCK950      = 9,                        // МПЦ EBILOCK950   - не используется, специфицируем в Confif
//      VERSION_RPCDIALOG       = 10,                       // РПЦ Диалог       - не используется, специфицируем в Confif
        VERSIONDcmpk            = 11,                       // ДЦ МПК
    };

    enum Common
    {
        MaxQueueLength          = 32,                       // макс.длина очередей
        MaxModule               = 48,                       // макс.номер модуля
        TsMaxLengthBits         = 1024 * 3,                 // макс.размер сигналов в битах 3072
        TsMaxLengthBytes        = (1024 * 3)/8,             // макс.размер сигналов в байтах 384
    };

    // открытые статические члены
    static QHash<int, Station*> Stations;                   // хэш-таблица указателей на справочники станций

    // открытые статические функции
    static Station * GetByNo(int no);                       // поучить справочник по номеру станции
    static Station * GetByName(QString stname);             // поучить справочник по номеру станции
    static bool ReadBd (QString& dbpath, Logger& logger);   // чтение БД
    static void sortTs();                                   // сортировка спимков ТС

    // открытые члены - таблицы ТС по станции
    QHash <QString, class Ts*> Ts;                          // индексированы по текстовому имени ТС
    QHash <int, class Ts*> TsIndexed;                       // индексированы по индексу ТС
    QHash <int, class Ts*> TsByIndxTsName;                  // индексированы по индексу имени
    QList <class Ts*> TsSorted;                             // отсортированы по имени

    // открытые функции
    Station(QSqlQuery& query, Logger& logger);              // конструктор на базе записи в БД
    ~Station();


    QString& Name() { return  name; }
    void AddTs (class Ts*, Logger& logger);                 // добавить ТС
    void GetTsParams (int& maxModul, int& maxI, int& maxJ, int& tsPerModule);

    bool IsMpcEbilock() { return mpcEbilock; }              // конфигурация с Ebilock950, а также РПЦ/МПЦ МПК + rpcMpcMPK
    bool IsRpcMpcMPK () { return rpcMpcMPK; }               // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool IsRpcDialog () { return rpcDialog; }               // конфигурация с РПЦ Диалог

    // доступ к состоянию ТС по смещению ТС в массиве
    bool TsSts      (int i)
        { return TsStsMoment(i) && !TsPulse(i); }            // состояние ТС (1 - если не мигает и в единице)
    bool TsStsMoment(int i) { return TestBit(tsSts,     i); }// состояние ТС по моменту
    bool TsDir      (int i) { return TestBit(tsStsDir,  i); }// состояние не нормализованное
    bool TsPulse    (int i) { return TestBit(tsStsPulse,i); }// состояние пульсации

// вычисление переменной - через обработку сигнала в слоте
public slots:
   void GetValue(QString& name, int& ret);                    // вычисление переменной в выражении формата ИМЯ_ТС[ИМЯ_ИЛИ_#НОМЕР_СТАНЦИИ]

private:

    int     no;                                             // номер
    QString noext;                                          // конфигурация подслушек (номер или номер и IP, например: 15 [192.168.1.13 1051]
    QString name;
    int     ras;                                            // номер станции связи
    int     addr;                                           // линейный адрес
    int     krugId;                                         // номер круга
    int     version;                                        // версия конфигурации станции (KpVersion)
    int     otuAddr;                                        // адрес в подсистеме ОТУ (СПОК/БРОК)
    QString config;                                         // строка конфигурации
    int     kpIdBase;                                       // ID КП, явно заданный в БД или 0
    int     esr;                                            // код ЕСР
    int     gidUralId;                                      // идентификация в ГИД УРАЛ
    bool    enable;                                         // OnOff
    bool    du;                                             // станция ДУ
    int     bitmapIdRss;                                    //
    int     radioIdRss;                                     //
    int     radioIdMnt;                                     //
    int     T[4];                                           // коэффициенты связи
    int     forms;                                          // число форм
    QString extForms;                                       // доп.формы
    QString orient;                                         // ориентация

    bool    mpcEbilock;                                     // конфигурация с Ebilock950, а также РПЦ/МПЦ МПК + rpcMpcMPK
    bool    rpcMpcMPK;                                      // конфигурация с РПЦ/МПЦ МПК (кроме того mpcEbilock = true)
    bool    rpcDialog;                                      // конфигурация с РПЦ Диалог

    // состояние сигналов ТС

    QBitArray tsStsDir;
    QBitArray tsStsPulse;
    QBitArray tsStsDirPrv;
    QBitArray tsStsPulsePrv;
    QBitArray tsSts;


    // закрытые функции
    bool parseNames (QString& srcname, Station*& st, QString& name); // разбор индексированных имен ТУ/ТС
    bool TestBit (QBitArray& bits, int index);              // проверка бита в битовом массиве
};

#endif // STATION_H
