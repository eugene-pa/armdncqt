#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "../common/tcpheader.h"

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

    void slotReadyRead      ();
    void slotConnected      ();
    void slotDisconnected   ();
    void slotError (QAbstractSocket::SocketError);

    void on_pushButtonStart_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
