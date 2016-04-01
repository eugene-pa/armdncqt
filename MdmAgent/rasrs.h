#ifndef RASRS_H
#define RASRS_H

#include "../common/defines.h"
#include "../common/blockingrs.h"

class RasRs : public BlockingRs
{
    Q_OBJECT
public:
    RasRs(QObject *parent = 0, BYTE marker=0, int maxlength=4048);
    ~RasRs();

    // виртуальные функции
    virtual void mainLoop();                                // основной цикл

    //bool open(QString config);
private:
    QByteArray prepareData();
    BYTE seans;
};

#endif // RASRS_H
