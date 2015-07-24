#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QMainWindow>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QComboBox>

#include "../shapes/shapeset.h"
#include "../shapes/shapetrnsp.h"
#include "../shapes/colorscheme.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../spr/station.h"
#include "../spr/ts.h"
#include "../spr/tu.h"
#include "../spr/properties.h"
#include "../spr/esr.h"
#include "../common/archiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionNewForm_triggered();

    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

    void on_action_RcInfo_triggered();
    void stationSelected(int index);                        // выбор станции в списке

    void on_action_StrlInfo_triggered();

    void on_action_SvtfInfo_triggered();

    void on_action_RouteInfo_triggered();

    void on_action_prgInfo_triggered();

    void on_action_trainsInfo_triggered();

    void on_action_stationsInfo_triggered();

    void on_action_TuInfo_triggered();

    void on_action_OtuInfo_triggered();

    void on_action_TsInfo_triggered();

private:
    Ui::MainWindow *ui;
    class QMdiArea *mdiArea;                                // виджет MDI

    QComboBox* StationsCmb;                                 // станции

    QLabel hostStatus;

    ClientTcp * clientTcp;                                  // подключение к потоку ТС
};

#endif // MAINWINDOW_H
