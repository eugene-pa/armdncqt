#ifndef PAMESSAGE_H
#define PAMESSAGE_H

#include <string>
#include <sstream>

class PaMessage
{
public:
    // источники
    enum Sourcer
    {
        srcActLine = 1,
        srcPsvLine,
        srcDspLine,
        srcDbgLine,
        srcMpcLine,
        srcRpcLine,
        srcECEMLine,
        // добавить ...
    };

    // операции/действия
    enum Action
    {
        typRcv   = 1,
        typRcvBt ,
        typeSnt  ,
        typeDown ,
        typeUp   ,
        typeTrace,                  // трассировка: вывод сообщения на консоль
        // добавить ...
    };

    // состояние
    enum Status
    {
        stsOK         = 0,
        stsErrTimeout ,
        stsErrFormat  ,
        stsErrCRC     ,
        stsErrOverhead,
        // добавить ...
    };

    // конструктор
    PaMessage(Sourcer, Action, Status, std::wstring text = L"", void * dataptr=nullptr, int length=0);
    Sourcer GetSource() { return src;    }
    Action  GetAction() { return action; }
    Status  GetStatus() { return sts;    }
    std::wstring GetSourceText();       //
    std::wstring GetTypeText();
    std::wstring GetStatusText();
    std::wstring GetText() { return msg; }
    std::wstring toWstring();
    void * GetData();
    int GetDataLength();

private:
    Sourcer src;
    Action  action;
    Status  sts;
    std::wstring msg;
    int     reserv;
    void *  data;
    int     datalength;
};

#endif // PAMESSAGE_H
