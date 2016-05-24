#ifndef BRIDGETCP_H
#define BRIDGETCP_H

#include <QDialog>
#include <QHostAddress>
#include <QStatusBar>
#include "../common/defines.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/servertcp.h"
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
    // уведомления от клиентов, подключающихся к удаленным серверам
    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

    // уведомления сервера
    void slotAcceptError(QAbstractSocket::SocketError socketError);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDataready     (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);

private:
    virtual void timerEvent(QTimerEvent *event);            // таймер
    void loadResources();

    QVector <class ClientTcp*> connections;                 // соединения

    Ui::BridgeTcp *ui;
    QStatusBar * bar;
    class QLabel * msg;
    ClientTcp * mainConnection;
    ClientTcp * rsrvConnection;
    ServerTcp * server;
};

#endif // BRIDGETCP_H
