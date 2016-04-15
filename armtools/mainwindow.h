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
#include <QSpinBox>
#include <QtMath>
#include <QLayout>
#include <QProcess>

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
    void timerEvent(QTimerEvent *event);

    static bool blackBoxMode;                               // включен режима просмотра архива

private slots:
    void on_actionBlackbox_triggered();
    void stationSelected(int index);                        // выбор станции
    void tsToggled(bool);                                   // щелчок флажка поиск изменений ТС
    void tsSelected();                                      // выбор ТС
    void linkToggled(bool);                                 // щелчок флажка поиск изменений ТС

    void connected   (ClientTcp *);                         // установлено соединение
    void disconnected(ClientTcp *);                         // разорвано соединение
    void error       (ClientTcp *);                         // ошибка сокета
    void dataready   (ClientTcp *);                         // готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
    void rawdataready(ClientTcp *);                         // получены необрамленные данные - отдельный сигнал


    void on_action_TS_triggered();

    void on_action_TU_triggered();

    void on_action_OTU_triggered();

    void on_action_Routes_triggered();

    void on_action_KP_triggered();

    void on_action_RC_triggered();

    void on_action_STRL_triggered();

    void on_action_SVTF_triggered();

    void on_action_Stations_triggered();

    void on_action_More_triggered();

    void on_action_Less_triggered();

    void on_action_ZoomOff_triggered();

    void on_action_ToolBar2_triggered();

    void scaleView();

    void on_actionPlay_triggered();

    void on_action_Stop_triggered();

    void on_action_Toolbar_triggered();

    void on_actionNext_triggered();

    void on_actionPrev_triggered();

signals:
    void changeStation(class Station *);                    // смена станции

private:
    Ui::MainWindow *ui;
    class ShapeChild * child;
//    qreal scale;                                            // масштаб

    QComboBox* StationsCmb;                                 // станции
//    QCheckBox* checkBox;
    QDateEdit* dateEdit;                                    // задание даты
    QTimeEdit* timeEdit;                                    // задание вресени
    QCalendarWidget* calendar;                              // пристегиваемый календарь выбора даты
    QSpinBox * stepValue;                                   // задание шага смещений в архиве, мин
    QLabel   * labelStep;
    QLabel   * labelTemp;                                   // Темп и диапазон
    QLabel   * labelTemp2;
    QSlider  * sliderTemp;                                  // слайдер темпа воспроизведения
    QSlider  * sliderScale;                                 // слайдер масштабирования

    QCheckBox* checkFindTs;                                 // флажок Поиск изменений ТС
    QComboBox* cmbTs;                                       // комбо-бокс выбора ТС
    QCheckBox* checkFindLink;                               // флажок Поиск изменений состояния связи

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
    class DlgStationsInfo * dlgStations;                    // станции

    // работа с архивом
    QDateTime arhDateTime;                                  // дата/время архивного файла
    ArhReader * reader;
    bool bPlay;                                             // воспроизведение вперед
    bool bPlayBack;                                         // воспроизведение назад
    int  idTimer;                                           // таймер воспроизведения
    bool readNext(bool findChanges=false);                  // прочитать и отобразить след.запись в архиве
    void readPrev();                                        // прочитать и отобразить пред.запись в архиве

    bool isExtFind() { return isFindTsChanges() || isFindLinkErrors(); }
    bool isFindTsChanges() { return checkFindTs->isChecked(); }     // режим поиска ТС ?
    bool isFindLinkErrors(){ return checkFindLink->isChecked(); }   // режим поиска ошибок связи ?
};

#endif // MAINWINDOW_H
