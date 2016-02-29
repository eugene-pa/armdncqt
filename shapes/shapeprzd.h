#ifndef SHAPEPRZD_H
#define SHAPEPRZD_H

#include "shape.h"

class ShapePrzd : public DShape
{
public:
    ShapePrzd(QString& src, ShapeSet* parent);
    ~ShapePrzd();
};

#endif // SHAPEPRZD_H
