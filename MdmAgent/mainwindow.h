#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QUdpSocket>
#include <QMessageBox>
#include <QTimer>
#include <sstream>
#include <thread>
#include <../common/defines.h>
#include <../common/logger.h>
#include <../common/blockingrs.h>
#include <../common/clienttcp.h>
#include <../common/servertcp.h>

extern QString configMain;                                  // строка конфигурации BlockingRS прямого канала
extern QString configRsrv;                                  // строка конфигурации BlockingRS обратного канала
extern bool armAcked;                                       // получена квитанция АРМ ДНЦ
extern std::condition_variable waterAck;                    // условие ожидания квитанции
extern std::condition_variable waterNet;                    // условие ожидания датаграмм
extern std::queue <unsigned char> dataInNet;                // входные данные, полученные по сети
extern std::mutex                 mtxDataNet;               // синхронизация доступа к dataInNet
extern unsigned int cycles;                                 // счетчик циклов всех станций
extern QTime       start;                                   // засечка начала цикла

void SendMessage (int, void *, void * p2 = nullptr);        // прототип глобальной функции отправки сообщения
void   ThreadPolling (long);                                // прототип глобальной функции потока опроса линии связи
//void Log (std::wstring);                                    // прототип глобальной функции вывода лога широкой строки


namespace Ui {
class MainWindow;
}


// класс-deleter для завершения рабочих потоков; используется в смарт-указателях std::unique_ptr
// выполняет: ожидание завершения, вывод в лог и удаление указателя
class ThreadTerminater
{
public:
    void operator () (std::thread * p)
    {
        //auto id = p->get_id();                                          // запоминаем id, пока поток живой
        p->join();                                                      // ожидаем завершения потока
        //std::wstringstream s;
        //s << L"Удаление указателя на поток " << id;                     // если хотим убедиться в удалении указателя
        //Log(s.str());                                                   // выводим лог
        qDebug() << "Удаление указателя на поток";
        delete p;                                                       // удаляем указатель
    }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Messages
    {
        MSG_LOG         = 1,                                    // лог сообщения
        MSG_SHOW_INFO   = 2,                                    // отобразить информацию об опрошенной станции
        MSG_SHOW_PING   = 3,                                    // отобразить информацию о точке опроса
        MSG_SHOW_SND    = 4,                                    // отобразить переданный пакет данных
        MSG_SHOW_RCV    = 5,                                    // отобразить принятый   пакет данных
        MSG_ERR         = 6,                                    // ошибка связи без уточнения
        MSG_ERR_TIMEOUT = 7,                                    // ошибка тайм-аута
        MSG_ERR_FORMAT  = 8,                                    // ошибка формата
        MSG_ERR_CRC     = 9,                                    // ошибка CRC
        MSG_SND_NET     = 10,                                   // передача пакета для КП в сеть (датаграмма)
    };

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    static MainWindow * mainWnd;
    static bool IsNetSupported();                               // поддержка сети

    void setCycles(unsigned int);                               // отобразить число циклов
    void setPeriod(unsigned int);                               // отобразить длит.цикла

signals:
    void SendMsg(int, void *, void *);

public slots:
    void SelectStation(class Station *);
    void GetMsg (int, void *, void *);

//    void dataready(QByteArray);                               // сигнал-уведомление о готовности данных
//    void timeout();                                           // сигнал-уведомление об отсутствии данных в канала данных
//    void error  (int);                                        // сигнал-уведомление об ошибке
//    void rsStarted();                                         // старт потока RS
    //void rsFinished();                                        // завершение потока RS

    void on_pushButtonMainOff_clicked();

    void on_pushButtonRsrvOff_clicked();

    void on_pushButtonToMain_clicked();

    void on_pushButtonToRsrv_clicked();

    void on_pushButtonTest_clicked();

    void on_pushButtonATU_clicked();

    void on_pushButtonReset_clicked();

    void on_pushButtonGetReconnect_clicked();

    void on_pushButtonResetMain_clicked();

    void on_pushButtonResetRsrv_clicked();

    void on_pushButtonWatchdog_clicked();

    // уведомления сервера
    void slotAcceptError      (ClientTcp *);
    void slotSvrNewConnection (ClientTcp *);
    void slotSvrDataready     (ClientTcp *);
    void slotSvrDisconnected  (ClientTcp *);
    void slotRoger            (ClientTcp *);

    // прием датаграмм
    void readDatagramm();

signals:
    void exit();
    void changeStation(class Station *);                                // смена станции

private slots:
    void on_action_KP_triggered();

    void on_checkBox_ack_stateChanged(int arg1);

    void on_checkBox_Full_stateChanged(int arg1);

private:
    virtual void timerEvent(QTimerEvent *event);                // обраьотка таймера

    Ui::MainWindow *ui;

    class Station * actualSt;                                   // актуальная станция (выбрана)
    std::wstring config;                                        // конфигурация порта
    std::unique_ptr<std::thread, ThreadTerminater> pThreadPolling;// smart-указатель на поток опроса динии связи
    class DlgKPinfo * dlgKp;
    void loadResources();

    int portTcp;                                                // порт подключений модулей Управление
    ServerTcp * server;                                         // сервер подключений модулей Управление

    void sendToAllClients(class StationNetTS*);                 // отправить данные всем подключенным клиентам, от которыъ было подтверждение

    QUdpSocket * sndSocket;                                     // сокет для передачи в КП датаграмм
    QUdpSocket * rcvSocket;                                     // сокет для приема датаграмм из КП
    int         portSnd;                                        // порт передачи датаграмм
    int         portRcv;                                        // порт приема датаграмм
    //class BlockingRS * rasRs;
    class SqlBlackBox * blackbox;                               // sql-протоколирование

    QTimer      timerAck;                                       // таймер отпраки квитанций работоспособноси клиентам Управление (опция ETPULSE)
    QTimer      timerOR;                                        // таймер отправки сообщений о работоспособности основно РСС
    QTimer      timerAutoswitch;                                // таймер отслеживания работоспособности аппаратуры и автопереключения РСС

    time_t      tUcSnd;                                         // засечка передачи из УЦ
};

#endif // MAINWINDOW_H
