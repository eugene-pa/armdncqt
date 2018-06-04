#ifndef SYSINFO_H
#define SYSINFO_H

#include "QByteArray"
#include "QDateTime"

#include "station.h"

// класс SysInfo, инкапсулирующий данные блока системной информации
/*
enum LinkErrors
{
    ok      = 0,
    timeout = 1,
    format  = 2,
    crc     = 3,
};
*/
class SysInfo
{
    friend class Station;
    friend class DStDataFromMonitor;
    friend class kpframe;
    friend class StationNetTS;

public:
    SysInfo();
    SysInfo(class Station * pst);
    void Parse(void *, int);                                // принять заданное число байт статуса
    ~SysInfo();

    // определяю макросы get/set для доступа к байтам массива по индексу
    #define get(i) (){ return src[i]; }                     // можно было функцией: inline BYTE get(int i) { return src[i]; }
    #define bit(i,j) () { return (src[i] & (0x01<<j)) > 0; }
    #define set(i) (BYTE bt){ src[i] = bt; }

    BYTE SysStatus        get(0)                            // Байт 0 - состояние SysStatus
    void SysStatus        set(0)
    bool Com3Connected    bit(0,0)                          // наличие соединения на COM3 (активный модем)
    bool Com4Connected    bit(0,1)                          // наличие соединения на COM4 (пассивный модем)
    bool Com3Error        bit(0,2)                          // ошибка инициализации модема на СОМ3
    bool Com4Error        bit(0,3)                          // ошибка инициализации модема на СОМ4
    bool Transitted       bit(0,4)                          // блок программно отключился от линии (сам)
    bool TsCompared       bit(0,5)                          // совпадение ТС основного и резервного блоков
    bool Compressed       bit(0,6)                          // передача сжатого пакета обновлений ТС
    bool OtuLineOk        bit(0,7)                          // ОМУЛ в норме

    int  SpeedCom3        () { return (src[1]&0x3f)*1200; } // Байт 1 - скорость Com3
    void SpeedCom3        set(1)                            //

    int  BreaksCom3       get(2)                            // Байт 2 - число реконнектов Com3
    void BreaksCom3       set(2)                            //

    int  SpeedCom4        () { return (src[3]&0x3f)*1200; } // Байт 3 - скорость Com4
    void SpeedCom4        set(3)                            //

    int  BreaksCom4       get(4)                            // Байт 4 - число реконнектов Com4
    void BreaksCom4       set(4)                            //

    BYTE MKUStatus        get(8)                            // Байт 8 - статус МКУ
    void MKUStatus        set(8)
    bool IsRsrv           bit(8,0)                          // Резервный БМ
    bool IsLineOnOne      bit(8,1)                          // Подключен к линии
    bool IsControlOne     bit(8,2)                          // Подключен к объекту
    bool IsLineOnAnother  bit(8,3)                          // Смежный подключен к линии
    bool IsControlAnother bit(8,4)                          // Смежный подключен к объекту
    bool IsMpcOk          bit(8,5)                          // Связь с МПЦ в норме
    bool IsRpcOk          bit(8,6)                          // Связь с РПЦ в норме
    bool IsTestMode       bit(8,7)                          // Режим тестирование модулей включен

    BYTE MVVStatus       get(12)                            // Байт 12 - статус МВВ1,МВВ2, связи
    void MVVStatus       set(12)
    bool ErrAtu1         bit(12,0)                          // Ош.АТУ МВВ1
    bool ErrKey1         bit(12,1)                          // Ош.ключа МВВ1
    bool ErrOut1         bit(12,2)                          // Ош.выхода МВВ1
    bool ErrAtu2         bit(12,3)                          // Ош.АТУ МВВ2
    bool ErrKey2         bit(12,4)                          // Ош.ключа МВВ2
    bool ErrOut2         bit(12,5)                          // Ош.выхода МВВ2
    bool OkAdkScb        bit(12,6)                          // Связь с АДК СЦБ
    bool OkApkDk         bit(12,7)                          // Связь с АПК ДК

    BYTE LoVersionNo     () { return  src[13]; }            // Байт 13 - младший байт номера версии
    void LoVersionNo     (BYTE value) { src[13] = value; }

    BYTE SysStatusEx     get(14)                            // Байт 14 - расширенный статус
    void SysStatusEx     set(14)
    bool ArmDspModeOn    bit(14,0)                          // АРМ ДСП
    bool DebugOtuMode    bit(14,1)                          // Режим отладки ОТУ
    bool DebugPortOn     bit(14,2)                          // Отладочный порт включен
    bool RetranslateMode bit(14,3)                          // режим ретрансляции точка-точка в КП
    bool MemoryLeak      bit(14,4)                          // свободной памяти < 1 Мб
    bool WatchDogOn      bit(14,5)                          // сторожевой таймер включен
    bool OtuBrokOn       bit(14,6)                          // выполнение ОТУ на БРОК
    bool BackPolling     bit(14,7)                          // опрос станции с обводного канала

    BYTE GetMtuMtsLineStatus(int i);                        // получить статус линейки из 8 модулей БТ индексу 0-5
    void SetMtuMtsLineStatus(int i, BYTE bte);              // установить статус линейки из 8 модулей БТ индексу 0-5
    bool MtuMtsStatus(int i);                               // проверка состояния модуля МТУ/МТС

    void SetLineStatus(LineStatus s) { linestatus = s; }
    QDateTime&  LastTime() { return tmdt; }                 // время опроса
    void FixTime();                                         // засечка времени опроса
    int  LinkErrors() { return errors; }                    // общее число ошибок связи с комплектом

    QByteArray& Src() { return src; }                       // маасив инфо-блока
    QString ErrorType();                                    // получить текст типа ошибки
private:
    class Station * st;
    // 0  байт - состояние КП
    // 1  байт - скорость COM3 + 2 бита резерва
    // 2  байт - число реконнектов на COM3
    // 3  байт - скорость COM4 + 2 бита резерва
    // 4  байт - число реконнектов на COM4
    // 5  байт - МВВ1: отказы МТС, МТУ
    // 6  байт - МВВ1: отказы МТС, МТУ
    // 7  байт - МВВ1: отказы МТС, МТУ
    // 8  байт - МКУ
    // 9  байт - МВВ2: отказы МТС, МТУ
    // 10 байт - МВВ2: отказы МТС, МТУ
    // 11 байт - МВВ2: отказы МТС, МТУ
    // 12 байт - статус МВВ1,МВВ2, связи
    // 13 байт - версия
    // 14 расширенный статус
    QByteArray src;                                         // исходный блок

    QDateTime  tmdt;                                        // время получения данных
    QDateTime  tmdtPrev;                                    // время получения данных в пред.цикле
    LineStatus linestatus;                                  // состояние приема из линии 0-OK, 1-таймаут,2-ош.формата
    UINT       errors;                                      // счетчик ошибок связи
    UINT       dt;                                          // продолжительность обмена, мсек

    #undef get
    #undef set
    #undef bit
};

#endif // SYSINFO_H
