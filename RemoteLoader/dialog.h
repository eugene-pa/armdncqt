#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QStringList>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QThread>

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
    QString serverConnectStr;                               // удаленный сервер
    QVector <BriefFileInfo> _files;                         // информация о файлах заданного каталога (если копируем каталог)
    int indx;                                               // индекс копируемого файла
    QString localDstPath;                                   // актуальный локальный путь
    BriefFileInfo info;                                     // информация об актуальном файле

    qint64  todo;                                           // общий размер файлов для копирования
    qint64  done;                                           // скопировано

    QStatusBar * statusBar;
    QLabel     * labelMsg;
    QLed       * status;

    ClientTcp * connection;                                 // соединение

    const int blocksize = 48000;
    const int breakT = 5000;                                // время ожидания соединения

    void rqReadFile(QString src, QString dst, qint64 offset, int length);
    virtual void timerEvent(QTimerEvent *event);                // таймер

    int idTimer;

    QString version;
};

#endif // DIALOG_H
