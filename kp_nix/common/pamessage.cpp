#include "common/tu.h"
#include "pamessage.h"

// Класс paMessage предназначен для передачи сообщений между компонентами ПО
// Класс создается источником в момент, когда надо передать сообщение
// Сообщения передаются с помощью функтора  std::function<void(const PaSender&, paMessage *)>
// Класс SendMessage реализует конкретный для используемой платформы механизм передачи
// Так, при использовании QT рекомендуется создать его производным от QObject и связать
// с основным виджетом отображения. Что и как ПО отображения будет делать с сообщением - не важно,
// возможно, при наличии разных виджетов для разных подсистем, будет выполнена дальнейша япересылка
// подключенным слотам дочерних виджетов
// Хотя возможны любые варианты взаимодействия, тот же сетевой вариант
// Главное: отправка сообщений в коде рабочих потоков будет независима от способа передачи!


PaMessage::PaMessage(Sourcer source, std::wstring text, Event act, Status status, void * dataptr, int length)
{
    src         = source;
    action      = act;
    sts         = status;
    msg         = text;
    data        = dataptr;
    datalength  = length;
    ack         = tuAckError;
    tu          = 0;
}

// упрощенный конструктор для лога
PaMessage::PaMessage(std::wstring text)
{
    src         = srcAny;
    action      = eventTrace;
    sts         = stsOK;
    msg         = text;
    data        = nullptr;
    datalength  = 0;
    ack         = tuAckError;
    tu          = 0;
}

// упрощенный конструктор для сообщений о процессе исполнения ТУ
PaMessage::PaMessage(AckTypes ack, std::shared_ptr<Tu> tu, std::wstring msg)
{
    src         = srcTU;
    action      = eventTu;
    sts         = stsOK;
    msg         = msg;
    data        = nullptr;
    datalength  = 0;
    this->ack   = ack;
    this->tu    = tu;
}

std::wstring PaMessage::GetSourceText()
{
    switch(src)
    {
        case srcActLine   : return L"COM3";
        case srcPsvLine   : return L"COM3";
        case srcDbgLine   : return L"COMDBG";
        case srcMpcLine   : return L"Ebolock";
        case srcRpcLine   : return L"РПЦ Диалог";
        case srcECEMLine  : return L"ЭЦ-ЕМ";
        case srcAdkScbLine: return L"АДКСЦБ";
        case srcApkdkLine : return L"АПКДК";
        case srcAbtcmLine : return L"АБТЦМ";
        case srcKabLine   : return L"КЭБ";
        case srcKvartzLine: return L"Кварц";
        default           : return L"Источник не определен";
    }
}

std::wstring PaMessage::GetTypeText()
{
    switch (action)
    {
        case eventTrace     : return L"Лог";
        case eventError     : return L"Ошибка";
        case eventReceive   : return L"Прием";
        case eventReceiveBt : return L"Прием байта";
        case eventSend      : return L"Передано";
        case eventDown      : return L"Отказ";
        case eventUp        : return L"Восстановление";

        default       : return L"Тип неопределен";
    }
}

std::wstring PaMessage::GetStatusText()
{
    switch (sts)
    {
        case stsOK             : return L"Норма";
        case stsErrTimeout     : return L"Таймаут ПРМ";
        case stsErrTimeoutSend : return L"Таймаут ПРД";
        case stsErrFormat      : return L"Ошбибка формата";
        case stsErrLength      : return L"Ошбибка длины";
        case stsErrCRC         : return L"Ошибка CRC";
        case stsErrOverhead    : return L"Переполнение";

        default      : return L"Состояние неопределено";
    }
}

std::wstring PaMessage::toWstring()
{
    std::wstringstream tmp;
    tmp << "Источник: " << GetSourceText() << ". Тип: " << GetTypeText() << ". Статус: " << GetStatusText();
    return tmp.str();
}

void * PaMessage::GetData()
{
    return data;
}

int PaMessage::GetDataLength()
{
    return datalength;
}
