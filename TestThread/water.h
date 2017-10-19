#ifndef WATER_H
#define WATER_H

#include <QObject>
#include "mainwindow.h"

class Water : public QObject
{
    Q_OBJECT
public:
    Water(QWaitCondition*);
    ~Water();
static bool rqfinish;

public slots:
    void waiting(int ms);

signals:
    void timeout();                                         // таймаут
    void evented(int);                                      // получено событие

private:
    QWaitCondition * condition;                             // условие ожидания данных
    //static QMutex watermutex;                                      // блокировка условия ожидания


    int n;
};

#endif // WATER_H
