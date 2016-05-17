#ifndef BRIDGETCP_H
#define BRIDGETCP_H

#include <QDialog>
#include <QHostAddress>
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/defines.h"
#include "../common/inireader.h"


extern Logger logger;
extern QString iniFile;

namespace Ui {
class BridgeTcp;
}

class BridgeTcp : public QDialog
{
    Q_OBJECT

public:
    explicit BridgeTcp(QWidget *parent = 0);
    ~BridgeTcp();

private:
    Ui::BridgeTcp *ui;
};

#endif // BRIDGETCP_H
