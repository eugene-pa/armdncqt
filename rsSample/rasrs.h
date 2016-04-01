#ifndef RASRS_H
#define RASRS_H

#include "defines.h"
#include "blockingrs.h"


class RasRS : public BlockingRs
{
    Q_OBJECT
public:
    RasRS(QObject *parent = 0, BYTE marker=0, int maxlength=4048);
    ~RasRS();

    // виртуальные функции
    virtual void mainLoop();                                // основной цикл

private:
    QByteArray prepareData();
    BYTE seans;

};

#endif // RASRS_H
