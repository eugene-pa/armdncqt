#ifndef DLGTRAINS_H
#define DLGTRAINS_H

#include <QDialog>

namespace Ui {
class DlgTrains;
}

class DlgTrains : public QDialog
{
    Q_OBJECT

public:
    explicit DlgTrains(QWidget *parent = 0);
    ~DlgTrains();

private slots:
    void on_checkBox_toggled(bool checked);

private:
    Ui::DlgTrains *ui;
    void fill(bool all=true);
};

#endif // DLGTRAINS_H
