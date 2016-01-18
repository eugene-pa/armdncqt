#ifndef BMINFOWIDGET_H
#define BMINFOWIDGET_H

#include <QWidget>

namespace Ui {
class bmInfoWidget;
}

class bmInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit bmInfoWidget(QWidget *parent = 0);
    ~bmInfoWidget();

    void updateKp();

private:
    Ui::bmInfoWidget *ui;
};

#endif // BMINFOWIDGET_H
