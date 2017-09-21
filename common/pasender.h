#ifndef PASENDER_H
#define PASENDER_H

#include <QObject>
#include "pamessage.h"

extern void Log(std::wstring);                              // прототип вывода лога

class PaSender : public QObject                              // наследуемся от QObject для поддержки сигналов и слотов
{
    Q_OBJECT

public:
    PaSender();
    void SendMsg(PaMessage *);
signals:
    void SendMessage(PaMessage *);
};

#endif // PASENDER_H
