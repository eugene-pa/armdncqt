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

    void UpdateQueues(class PaMessage);
    void UpdateQueueTodo(DWORD tu);
    void UpdateQueueDone(DWORD tu);
    void UpdateQueueSysy(DWORD tu);

private:
    Ui::FrameTU *ui;
};

#endif // FRAMETU_H
