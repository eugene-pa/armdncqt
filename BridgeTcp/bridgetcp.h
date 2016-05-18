#ifndef BRIDGETCP_H
#define BRIDGETCP_H

#include <QDialog>
#include <QHostAddress>
#include <QStatusBar>
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/defines.h"
#include "../common/inireader.h"


extern Logger logger;
extern QString iniFile;
extern bool compressEnabled;

namespace Ui {
class BridgeTcp;
}

class BridgeTcp : public QDialog
{
    Q_OBJECT

public:
    explicit BridgeTcp(QWidget *parent = 0);
    ~BridgeTcp();

private slots:
    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

private:
    Ui::BridgeTcp *ui;
    QStatusBar * bar;
    class QLabel * msg;
    ClientTcp * mainConnection;
    ClientTcp * rsrvConnection;
};

#endif // BRIDGETCP_H
