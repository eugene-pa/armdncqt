#ifndef PASENDER_H
#define PASENDER_H

#include <QObject>
#include "pamessage.h"

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
