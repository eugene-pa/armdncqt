#ifndef FARMELINE_H
#define FARMELINE_H

#include <QGroupBox>

namespace Ui {
class FarmeLine;
}

class FarmeLine : public QGroupBox
{
    Q_OBJECT

public:
    explicit FarmeLine(QWidget *parent = 0);
    ~FarmeLine();

private:
    Ui::FarmeLine *ui;
};

#endif // FARMELINE_H
