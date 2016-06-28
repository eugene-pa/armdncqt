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
#include "../paServer/remoterq.h"
//#include "../paServer/responceabout.h"
//#include "../paServer/responcedirs.h"
//#include "../paServer/responcefileinfo.h"
//#include "../paServer/responcefiles.h"
//#include "../paServer/responcetempfile.h"
//#include "../paServer/responceread.h"
//#include "../paServer/responcedrives.h"
//#include "../paServer/responceerror.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void ReadNext (ResponceRead&);                          // чтение файла

private slots:
    // уведомления от клиентов, подключающихся к удаленным серверам
    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

    void slotReadNext (ResponceRead&);                      // чтение файла

    void on_actionAbout_triggered();

    void on_pushButton_clicked();

    void on_actionDirs_triggered();

    void on_actionFiles_triggered();

    void on_actionFileUnfo_triggered();

    void on_actionRead_triggered();

    void on_actionTempCopy_triggered();

    void on_actionDrives_triggered();

    void on_lineEditFolder_textChanged(const QString &arg1);

    void on_lineEdit_windowIconTextChanged(const QString &iconText);

    void on_lineEdit_editingFinished();

private:
    Ui::MainWindow *ui;
    ClientTcp * connection;                                 // соединение
    class QLabel * msg;

    void rqReadFile(QString src, QString dst, qint64 offset, int length);

};

#endif // MAINWINDOW_H
