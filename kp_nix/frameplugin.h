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
    class QLed * modules[11];                       // массив укзателей на классы для индикаторов
};

#endif // FRAMEPLUGIN_H
