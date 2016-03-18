#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextCodec>
#include <QMainWindow>
#include <QFileDialog>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QCalendarWidget>
#include <QCheckBox>

#include "../shapes/shapeset.h"
#include "../shapes/shapetrnsp.h"
#include "../shapes/colorscheme.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../spr/station.h"
#include "../spr/ts.h"
#include "../spr/tu.h"
#include "../spr/krug.h"
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
    void on_actionBlackbox_triggered();
    void stationSelected(int index);

    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал

signals:
    void changeStation(class Station *);                    // смена станции

private:
    Ui::MainWindow *ui;
    class ShapeChild * child;

    QComboBox* StationsCmb;                                 // станции
    QCheckBox* checkBox;
    QDateEdit* dateEdit;
    QTimeEdit* timeEdit;
    QCalendarWidget* calendar;

    QLabel hostStatus;

    class ClientTcp * clientTcp;                                  // подключение к потоку ТС
    void loadResources();

    class DlgTsInfo * dlgTs;                                // состояние ТС
    class DlgTuInfo * dlgTu;                                // состояние ТС
    class DlgRcInfo * dlgRc;                                // состояние РЦ
    class DlgStrlInfo * dlgStrl;                            // состояние стрелок
    class DlgKPinfo * dlgKp;                                // состояние  КП
    class DlgRoutes * dlgRoutes;                            // состояние  маршрутов
    class DlgTrains * dlgTrains;                            // поезда

};

#endif // MAINWINDOW_H
