#ifndef OBJMAIN_H
#define OBJMAIN_H

#include <QObject>
#include "../common/pasender.h"
#include "../common/pamessage.h"

void threadsafecout(std::wstring msg);                           // безопасный (с блокировкой мьютекса) вывод строки на консоль
void Cleanup();

class ObjMain : public QObject
{
    Q_OBJECT
public:
    explicit ObjMain(QObject *parent = nullptr);

signals:

public slots:
    void GetMessage(PaMessage *msg);
    void Quit();
};

#endif // OBJMAIN_H
