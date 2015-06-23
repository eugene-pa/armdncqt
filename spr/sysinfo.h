#ifndef SYSINFO_H
#define SYSINFO_H

#include "QByteArray"
#include "QDateTime"

#include "enums.h"
#include "../common/defines.h"


// класс SysInfo, инкапсулирующий данные блока системной информации

class SysInfo
{
    friend class Station;
    friend class DStDataFromMonitor;
public:
    SysInfo();
    SysInfo(class Station * pst);
    ~SysInfo();

    BYTE SysStatus     () { return  src[0]; }               // Байт 0 - состояние SysStatus
    void SysStatus     (BYTE value) { src[0] = value; }     //
    BYTE MKUStatus     () { return  src[12]; }              // Байт 8 - статус МКУ
    BYTE MVVStatus     () { return  src[12]; }              // Байт 12 - статус МВВ
    BYTE SysStatusEx   () { return  src[14]; }              // Байт 14 - расширенный статус
    bool Com3Connected () { return (src[0] & 0x01) > 0; }   // наличие соединения на COM3 (активный модем)
    bool Com4Connected () { return (src[0] & 0x02) > 0; }   // наличие соединения на COM4 (пассивный модем)
    bool Com3Error     () { return (src[0] & 0x04) > 0; }   // ошибка инициализации модема на СОМ3
    bool Com4Error     () { return (src[0] & 0x08) > 0; }   // ошибка инициализации модема на СОМ4
    bool Transitted    () { return (src[0] & 0x10) > 0; }   // блок программно отключился от линии (сам)
    bool TsCompared    () { return (src[0] & 0x20) > 0; }   // совпадение ТС основного и резервного блоков
    bool Compressed    () { return (src[0] & 0x40) > 0; }   // передача сжатого пакета обновлений ТС
    bool OtuLineOk     () { return (src[0] & 0x80) > 0; }   // ОМУЛ в норме

    int  SpeedCom3     () { return (src[1]&0x3f) * 1200; }  // Байт 1 - скорость Com3
    void SpeedCom3     (BYTE value) { src[1] = value; }     //

    int  BreaksCom3    () { return src[2]; }                // Байт 2 - число реконнектов Com3
    void BreaksCom3    (BYTE value) { src[2] = value; }     //

    int  SpeedCom4     () { return (src[3]&0x3f) * 1200; }  // Байт 3 - скорость Com4
    void SpeedCom4     (BYTE value) { src[3] = value; }     //

    int  BreaksCom4    () { return src[4]; }                // Байт 4 - число реконнектов Com4
    void BreaksCom4    (BYTE value) { src[4] = value; }     //

                                                            // Байт 8 - MkuStatus
    bool IsRsrv          () { return (src[8 ] & 0x01) > 0; }// Резервный БМ
    bool IsLineOnOne     () { return (src[8 ] & 0x02) > 0; }// Подключен к линии
    bool IsControlOne    () { return (src[8 ] & 0x04) > 0; }// Подключен к объекту
    bool IsLineOnAnother () { return (src[8 ] & 0x08) > 0; }// Смежный подключен к линии
    bool IsControlAnother() { return (src[8 ] & 0x10) > 0; }// Смежный подключен к объекту
    bool IsMpcOk         () { return (src[8 ] & 0x20) > 0; }// Связь с МПЦ в норме
    bool IsRpcOk         () { return (src[8 ] & 0x40) > 0; }// Связь с РПЦ в норме
    bool IsTestMode      () { return (src[8 ] & 0x80) > 0; }// Режим тестирование модулей включен
                                                            // Байт 12 - статус МВВ1,МВВ2, связи
    bool ErrAtu1         () { return (src[12] & 0x01) > 0; }// Ош.АТУ МВВ1
    bool ErrKey1         () { return (src[12] & 0x02) > 0; }// Ош.ключа МВВ1
    bool ErrOut1         () { return (src[12] & 0x04) > 0; }// Ош.выхода МВВ1
    bool ErrAtu2         () { return (src[12] & 0x08) > 0; }// Ош.АТУ МВВ2
    bool ErrKey2         () { return (src[12] & 0x10) > 0; }// Ош.ключа МВВ2
    bool ErrOut2         () { return (src[12] & 0x20) > 0; }// Ош.выхода МВВ2
    bool OkAdkScb        () { return (src[12] & 0x40) > 0; }// Связь с АДК СЦБ
    bool OkApkDk         () { return (src[12] & 0x80) > 0; }// Связь с АПК ДК
    BYTE LoVersionNo     () { return  src[13]; }            // Байт 13 - младший байт номера версии
                                                            // Байт 14 - расширенный статус
    bool ArmDspModeOn    () { return (src[14] & 0x01) > 0; }// АРМ ДСП
    bool DebugOtuMode    () { return (src[14] & 0x02) > 0; }// Режим отладки ОТУ
    bool DebugPortOn     () { return (src[14] & 0x04) > 0; }// Отладочный порт включен
    bool RetranslateMode () { return (src[14] & 0x08) > 0; }// режим ретрансляции точка-точка в КП
    bool MemoryLeak      () { return (src[14] & 0x10) > 0; }// свободной памяти < 1 Мб
    bool WatchDogOn      () { return (src[14] & 0x20) > 0; }// сторожевой таймер включен
    bool OtuBrokOn       () { return (src[14] & 0x40) > 0; }// выполнение ОТУ на БРОК
    bool BackPolling     () { return (src[14] & 0x80) > 0; }// опрос станции с обводного канала

    BYTE GetMtuMtsStatus(int i);                            // получить статус модулей БТ индексу 0-5
    void SetMtuMtsStatus(int i, BYTE bte);                  // установить статус модулей БТ индексу 0-5

    QByteArray& Src() { return src; }

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
};

#endif // SYSINFO_H
