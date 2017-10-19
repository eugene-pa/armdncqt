#ifndef FRAMETU_H
#define FRAMETU_H

#include <QFrame>
#include "common/common.h"

namespace Ui {
class FrameTU;
}

class FrameTU : public QFrame
{
    Q_OBJECT

public:
    explicit FrameTU(QWidget *parent = 0);
    ~FrameTU();

    void UpdateQueues(class PaMessage *);
    void UpdateQueueBeg (std::shared_ptr<class Tu>);
    void UpdateQueueDone(std::shared_ptr<class Tu>);

    void SelectRow(int row, QColor clr);

private:
    Ui::FrameTU *ui;
    int FindById(DWORD id);
};

#endif // FRAMETU_H
