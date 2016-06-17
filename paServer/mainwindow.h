#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostInfo>
#include <QSysInfo>
#include <QTextCodec>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>

#include "../common/defines.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/servertcp.h"
#include "../common/defines.h"
#include "../common/inireader.h"
#include "../forms/qled.h"
#include "../common/inireader.h"
#include "remoterq.h"
#include "responceabout.h"
#include "responcedirs.h"
#include "responcefileinfo.h"
#include "responcefiles.h"
#include "responcetempfile.h"
#include "responceread.h"
#include "responcedrives.h"
#include "responceerror.h"

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
    // уведомления сервера
    void slotAcceptError      (ClientTcp *);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDataready     (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);

    // уведомления клиентов рекурсивных подключений
    void nextConnected        (ClientTcp *);
    void nextdisconnected     (ClientTcp *);
    void nexterror            (ClientTcp *);
    void nextdataready        (ClientTcp *);

    void on_actionIni_triggered();

    void on_actionLog_triggered();

    void on_actionAbout_triggered();

    void on_actionQT_about_triggered();

private:
    Ui::MainWindow *ui;

    class QLabel * portInfo;
    class QLed   * serverStatus;
    class QLabel * msg;
    ServerTcp * server;
    QList <class ClientTcp*> _trash;                        // корзина отработанных клиентов

    void loadResources();

    int findRowByConn(ClientTcp *);

    virtual void timerEvent(QTimerEvent *event);

    QProcess processIni;
    QProcess processLog;
};

#endif // MAINWINDOW_H
