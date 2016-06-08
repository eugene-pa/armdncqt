#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostInfo>
#include <QSysInfo>
#include <QTextCodec>
#include <QLabel>

#include "../common/defines.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../common/servertcp.h"
#include "../common/defines.h"
#include "../common/inireader.h"
#include "remoterq.h"
#include "responceabout.h"
#include "responcedirs.h"
#include "responcefileinfo.h"
#include "responcefiles.h"
#include "responcetempfile.h"
#include "responceread.h"
#include "responcedrives.h"

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
    void slotAcceptError(QAbstractSocket::SocketError socketError);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDataready     (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);

private:
    Ui::MainWindow *ui;
    class QLabel * msg;
    ServerTcp * server;

    void MainWindow::loadResources();
};

#endif // MAINWINDOW_H
