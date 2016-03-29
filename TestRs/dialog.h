#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include "settingsdialog.h"
#include "rsbase.h"

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
    void on_pushButton_clicked();
    void on_pushButton_Open_clicked();
    void applysettings();

signals:
    void operate(class RsBase*);

private:
    Ui::Dialog *ui;
    SettingsDialog * settingdlg;
    class RsBase *serial;
    QThread readerThread;
    class MdmAgentReader * worker;
};


class MdmAgentReader : public QObject
{
    Q_OBJECT
public slots:
    int readData(class RsBase*);
signals:
    void resultReady(const QString &result);
    void error(int error);
};

#endif // DIALOG_H
