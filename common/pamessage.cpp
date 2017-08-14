#include "pamessage.h"

// Класс paMessage предназначен для передачи сообщений между компонентами ПО
// Класс создается источником в момент, когда надо передать сообщение
// Сообщения передаются с помощью функтора  std::function<void(const SendMessage&, paMessage *)>
// Класс SendMessage реализует конкретный для используемой платформы механизм передачи
// Так, при использовании QT рекомендуется создать его производным от QObject и связать
// с основным виджетом отображения. Что и как ПО отображения будет делать с сообщением - не важно,
// возможно, при наличии разных виджетов для разных подсистем, будет выполнена дальнейша япересылка
// подключенным слотам дочерних виджетов
// Хотя возможны любые варианты взаимодействия, тот же сетевой вариант
// Главное: отправка сообщений в коде рабочих потоков будет независима от способа передачи!

paMessage::paMessage(Sourcer source, Action act, Status status, void * dataptr, int length)
{
    src         = source;
    action      = act;
    sts         = status;
    data        = dataptr;
    datalength  = length;
}

std::wstring paMessage::GetSource()
{
    switch(src)
    {
        case srcActLine : return L"COM3";
        case srcPsvLine : return L"COM3";
        case srcDbgLine : return L"COMDBG";
        case srcMpcLine : return L"Ebolock";
        case srcRpcLine : return L"РПЦ Диалог";
        case srcECEMLine: return L"ЭЦ ЕМ";

        default:
                          return L"Источник не определен";
    }
}

std::wstring paMessage::GetType()
{
    switch (action)
    {
        case typRcv   : return L"Прием";
        case typRcvBt : return L"Прием байта";
        case typeSnt  : return L"Передано";
        case typeDown : return L"Отказ";
        case typeUp   : return L"Восстановление";
        case typeTrace: return L"Лог";

        default       : return L"Тип неопределен";
    }
}

std::wstring paMessage::GetStatus()
{
    switch (sts)
    {
        case stsOK          : return L"Норма";
        case stsErrTimeout  : return L"Таймаута";
        case stsErrFormat   : return L"Ошбибка формата";
        case stsErrCRC      : return L"Ошибка CRC";
        case stsErrOverhead : return L"Переполнение";

        default      : return L"Состояние неопределено";
    }
}

std::wstring paMessage::toWstring()
{
    std::wstringstream tmp;
    tmp << "Источник: " << GetSource() << ". Тип: " << GetType() << ". Статус: " << GetStatus();
    return tmp.str();
}

void * paMessage::GetData()
{
    return data;
}

int paMessage::GetDataLength()
{
    return datalength;
}
