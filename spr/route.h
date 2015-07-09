#ifndef ROUTE_H
#define ROUTE_H

#include "../common/logger.h"
#include "sprbase.h"
//#include "properties.h"

class Route : public SprBase
{
    friend class DStDataFromMonitor;                        // для формирования и извлечения информации в потоке ТС
    friend class DDataFromMonitor;

public:
    Route();
    ~Route();
};

#endif // ROUTE_H
