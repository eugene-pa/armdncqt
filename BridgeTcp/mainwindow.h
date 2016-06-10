#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QProcess>
#include "../common/defines.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/servertcp.h"
#include "../common/defines.h"
#include "../common/inireader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

    void on_action_view_ini_triggered();

    void on_action_view_log_triggered();

    void on_action_about_triggered();

private:
    virtual void timerEvent(QTimerEvent *event);            // таймер
    void loadResources();
    int findRowByConn(ClientTcp *conn);

    QVector <class ClientTcp*> connections;                 // соединения

    class QLabel * msg;
    ClientTcp * mainConnection;
    ClientTcp * rsrvConnection;
    ServerTcp * server;

    QProcess processIni;
    QProcess processLog;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
