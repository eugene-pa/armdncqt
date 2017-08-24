#ifndef PAMESSAGE_H
#define PAMESSAGE_H

#include <string>
#include <sstream>
#include "common/common.h"

void Log (std::wstring s);                                  // лог

class PaMessage
{
public:
    // источники
    enum Sourcer
    {
        srcAny     = 0,             // общее сообщение
        srcActLine    ,             // активный модем
        srcPsvLine    ,             // массивный модем
        srcDspLine    ,             // обмен с АРМ ДСП
        srcDbgLine    ,             // отладочный порт
        srcMpcLine    ,             // связь с МПЦ Ebilock (первый канал)
        srcRpcLine    ,             // связь с РПЦ Диалог
        srcECEMLine   ,             // связь с ЭЦЕМ
        srcECMPKLine  ,             // связь с ЭЦ/МПЦ МПК
        srcAdkScbLine ,             // связь с АДК СЦБ
        srcApkdkLine  ,             // связь с АПКДК
        srcAbtcmLine  ,             // связь с АБТЦМ
        srcKabLine    ,             // связь с КЭБ
        srcKvartzLine ,             // связь с Кварц
        srcTU         ,             // тракт ТУ
        srcOTU        ,             // тракт ОТУ
        // добавить ...
    };

    // операции/действия
    enum Event
    {
        eventTrace = 1,             // вывод сообщения в лог (на консоль)
        eventError    ,             // сообщение об ошибке
        eventReceive  ,             // прием блока данных
        eventReceiveBt,             // прием очередного байта
        eventSend     ,             // данные переданы
        eventDown     ,             // потеря связи
        eventUp       ,             // восстановление связи
        eventAck      ,             // информация о квитанции

        // добавить ...
    };

    // состояние
    enum Status
    {
        stsOK         = 0 ,         // норма (ОК)
        stsErrTimeout     ,         // ошибка таймаута при приеме
        stsErrTimeoutSend ,         // ошибка таймаута при передаче
        stsErrFormat      ,         // ошибка формата пакета
        stsErrLength      ,         // ошибка длины
        stsErrCRC         ,         // ошибка CRC
        stsErrOverhead    ,         // переполнение
        // добавить ...
    };

    // конструктор
    PaMessage(Sourcer, std::wstring text, Event=eventTrace, Status=stsOK, void * dataptr=nullptr, int length=0);    // общий конструктор
    PaMessage(std::wstring text);                                                                                   // конструктор для лога
    PaMessage(int ack, DWORD tu);
    virtual ~PaMessage()
    {
        // Log (L"Удаляем объект PaMessage");   // если хотим убедиться в удалении сообщения
    }

    // доступ к свойствам
    Sourcer GetSource() { return src;    }
    Event  GetAction() { return action; }
    Status  GetStatus() { return sts;    }
    std::wstring GetSourceText();       //
    std::wstring GetTypeText();
    std::wstring GetStatusText();
    std::wstring GetText() { return msg; }
    void * GetData();
    int GetDataLength();
    // полное символьное представление сообщения
    std::wstring toWstring();

private:
    Sourcer src;                    // источник сообщения (задача)
    Event   action;                 // событие
    Status  sts;                    // состояние
    std::wstring msg;               // строка сообщения (возможно, пустая)
    int     ack;                    // код квитанции
    DWORD   tu;                     // код ТУ
    void *  data;                   // указатель на доп. данные
    int     datalength;             // длина доп.данных
};

#endif // PAMESSAGE_H
