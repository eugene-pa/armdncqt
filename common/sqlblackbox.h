#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H

#include "QString"
#include "QDateTime"
#include "vector"
#include "defines.h"
#include "sqlserver.h"
#include "logger.h"
#include "sqlserver.h"


class SqlBlackBox
{
public:

    SqlBlackBox (QString mainstr, QString rsrvstr="", Logger *logger=nullptr);
    SqlBlackBox (std::vector<QString> connstr, Logger *logger=nullptr);
    ~SqlBlackBox();
private:
    std::vector<class SqlServer*> servers;
};

#endif // SQLBLACKBOX_H
