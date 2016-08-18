#include "water.h"

bool Water::rqfinish = false;                   // блокировка условия ожидания
//QMutex Water::watermutex;                       // статический мьютекс иллюстрирует возможность разделения одного мьютекса несколькими потоками

Water::Water(QWaitCondition* _condition)
{
    qDebug() << "Water()";
    n = 0;
    condition = _condition;
}

Water::~Water()
{
    qDebug() << "~Water()";
}

void Water::waiting(int ms)
{
    qDebug() << "start Water::waiting";
    while (!rqfinish)
    {
        QMutex watermutex;                       // статический мьютекс иллюстрирует возможность разделения одного мьютекса несколькими потоками
        watermutex.lock();
        bool ret = condition->wait(&watermutex, ms);
        watermutex.unlock();

        if (ret)
            emit evented(++n);
        else
            emit timeout();
    }
    qDebug() << "end Water::waiting";
}

