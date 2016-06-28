#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStringList>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../paServer/remoterq.h"
#include "../forms/qled.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QStringList&, QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_buttonBox_rejected();

    // уведомления от клиентов, подключающихся к удаленным серверам
    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
//    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал
    void slotReadNext(ResponceRead&);                       // чтение файла

signals:
    void ReadNext (ResponceRead&);                          // чтение файла

private:
    Ui::Dialog *ui;
    QStringList params;
    QStatusBar * statusBar;
    QLabel     * labelMsg;
    QLed       * status;

    ClientTcp * connection;                                 // соединение

    const int blocksize = 48000;

    void rqReadFile(QString src, QString dst, qint64 offset, int length);
};

#endif // DIALOG_H
