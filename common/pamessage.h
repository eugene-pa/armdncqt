#ifndef PAMESSAGE_H
#define PAMESSAGE_H

#include <string>
#include <sstream>

class paMessage
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
    enum Action
    {
        typRcv   = 1,
        typRcvBt ,
        typeSnt  ,
        typeDown ,
        typeUp   ,
        typeTrace,              // трассировка: вывод сообщения на консоль
        // добавить ...
    };
    enum Status
    {
        stsOK         = 0,
        stsErrTimeout ,
        stsErrFormat  ,
        stsErrCRC     ,
        stsErrOverhead,
        // добавить ...
    };

    paMessage(Sourcer, Action, Status, void * dataptr=nullptr, int length=0);
    std::wstring GetSource();
    std::wstring GetType();
    std::wstring GetStatus();
    std::wstring toWstring();
    void * GetData();
    int GetDataLength();
private:
    Sourcer src;
    Action  action;
    Status  sts;
    int     reserv;
    void *  data;
    int     datalength;
};

#endif // PAMESSAGE_H
