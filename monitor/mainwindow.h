#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPushButton>
#include <QFileDialog>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QTextCodec>
#include <QTimeEdit>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QtMath>
#include <QLineEdit>
#include <QLayout>
#include <QProcess>
#include <QMessageBox>
#include <QScrollBar>
#include <QDir>

#include "../shapes/shapeset.h"
#include "../shapes/shapetrnsp.h"
#include "../shapes/colorscheme.h"
#include "../common/logger.h"
#include "../common/clienttcp.h"
#include "../spr/station.h"
#include "../spr/ts.h"
#include "../spr/tu.h"
#include "../spr/otu.h"
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
    void timerEvent(QTimerEvent *event);

private slots:
    void on_action_triggered();

    void on_action_2_triggered();
    void on_action_About_triggered();

private:
    Ui::MainWindow *ui;

    class ShapeChild * child;                               // активная схема

    void loadResources();                                   // загрузка ресурсов
    void stationSelected(ShapeId *);                        // выбор станции

    // вспомогательные диалоги
    class DlgTsInfo * dlgTs;                                // состояние ТС
    class DlgTuInfo * dlgTu;                                // список ТY
    class DlgOtu    * dlgOtu;                               // список ОТУ
    class DlgRcInfo * dlgRc;                                // состояние РЦ
    class DlgStrlInfo * dlgStrl;                            // состояние стрелок
    class DlgSvtfInfo * dlgSvtf;                            // состояние светофоров
    class DlgKPinfo * dlgKp;                                // состояние  КП
    class DlgRoutes * dlgRoutes;                            // состояние  маршрутов
    class DlgTrains * dlgTrains;                            // поезда
    class DlgStationsInfo * dlgStations;                    // станции
    class DlgPeregonInfo* dlgPeregons;                      // перегоны
    class DlgPereezd* dlgPereezd;                           // переезды

    QToolBar * extBar;                                      // доп.тулбар
    QFont * fontToolbar;                                    // шрифт доп. тулбара

    bool tooltip;                                           // вкл.откл. тултипы
};

#endif // MAINWINDOW_H
