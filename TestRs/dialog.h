#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include "settingsdialog.h"
#include "rsbase.h"
#include "../common/blockingrs.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void applysettings();
    void on_pushButton_clicked();
    void on_pushButton_Open_clicked();
    void dataready(QByteArray);                             // сигнал-уведомление о готовности данных
    void timeout();
    void error  (int);

    void on_Dialog_finished(int result);
    void rsStarted();
    void rsFinished();
signals:
    //void operate(/*class RsBase* */QString );
    void exit();

private:
    Ui::Dialog *ui;
    SettingsDialog * settingdlg;
//  class RsBase *serial;
    QThread readerThread;
    class MdmAgentReader * worker;

    class BlockingRs * rs;
    QString GetHex(QByteArray& array, int maxlength);
};


class MdmAgentReader : public QObject
{
    Q_OBJECT
public slots:
    int readData(/*class RsBase* */QString settings);
signals:
    void resultReady(const QString &result);
    void error(int error);
private:
    class RsBase *serial;
};

#endif // DIALOG_H
