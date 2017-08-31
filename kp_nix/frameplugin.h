#ifndef FRAMEPLUGIN_H
#define FRAMEPLUGIN_H

#include <QGroupBox>

namespace Ui {
class FramePlugin;
}

class FramePlugin : public QGroupBox
{
    Q_OBJECT

public:
    explicit FramePlugin(QWidget *parent = 0);
    ~FramePlugin();

private:
    Ui::FramePlugin *ui;
};

#endif // FRAMEPLUGIN_H
