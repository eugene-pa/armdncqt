#include "bridgetcp.h"
#include "ui_bridgetcp.h"

Logger logger("Log/shaper.txt", true, true);                // лог

#ifdef Q_OS_WIN
QString iniFile = "c:/armdncqt/bridgetcp/bridgetcp.ini";                    // настройки
#endif
#ifdef Q_OS_MAC
QString iniFile = "/Users/evgenyshmelev/armdncqt/bridgetcp/bridgetcp.ini";  // настройки
#endif
#ifdef Q_OS_LINUX
QString iniFile = "/home/eugene/QTProjects//bridgetcp/bridgetcp.ini";       // настройки
#endif

bool compressEnabled = true;                                // сжатие на летк
QString     mainServerConnectStr;                           // ip:порт основного сервера
QString     rsrvServerConnectStr;                           // ip:порт резервного сервера или ""
QHostAddress *ipMain;                                        //
QHostAddress *ipRsrv;

BridgeTcp::BridgeTcp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BridgeTcp)
{
    ui->setupUi(this);

    bar = new QStatusBar(this);
    ui->horizontalLayoutBar->addWidget(bar);
    bar->addWidget(msg = new QLabel("123"));
    msg->setFrameStyle(QFrame::NoFrame);


    IniReader rdr(iniFile);
    QString s;
    rdr.GetText("SERVER", s);
    QStringList addrs = s.split(QRegExp("[ ,;]+"));
    ui->lineEdit_main->setText(mainServerConnectStr = addrs.length() > 0 ? addrs[0] : "");
    ui->lineEdit_rsrv->setText(rsrvServerConnectStr = addrs.length() > 1 ? addrs[1] : "");

    QString ip = "";
    int port = 0;

    TcpHeader::ParseIpPort(mainServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipMain = mainServerConnectStr.length() ? new QHostAddress(mainServerConnectStr) : nullptr;
    mainConnection = new ClientTcp(mainServerConnectStr, &logger, false, "Шлюз СПД");
    QObject::connect(mainConnection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    ui->label_main->set(QLed::round, QLed::on, Qt::yellow);
    mainConnection->start();

    TcpHeader::ParseIpPort(rsrvServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipRsrv = rsrvServerConnectStr.length() ? new QHostAddress(rsrvServerConnectStr) : nullptr;
    if (ipRsrv != nullptr)
    {
        rsrvConnection = new ClientTcp(rsrvServerConnectStr, &logger, false, "Шлюз СПД");
        QObject::connect(rsrvConnection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
        ui->label_rsrv->set(QLed::round, QLed::on, Qt::yellow);
        rsrvConnection->start();
    }
    else
        ui->label_rsrv->set(QLed::round, QLed::off);

    QTableWidget * t = ui->tableWidget;
    t->setSortingEnabled(false);                             // запрещаем сортировку
    t->setColumnCount(5);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "Хост" << "Тип" << "IP адрес" << "Передано" << "Принято");

    t->setSortingEnabled(true);                             // разрешаем сортировку
    t->sortByColumn(0, Qt::AscendingOrder);                 // сортировка по умолчанию
    t->resizeColumnsToContents();

}

BridgeTcp::~BridgeTcp()
{
    delete ui;
}

// установлено соединение
void BridgeTcp::connected   (ClientTcp * conn)
{
    if (conn==mainConnection)
        ui->label_main->set(QLed::round, QLed::on, Qt::green);
    else
        ui->label_rsrv->set(QLed::round, QLed::on, Qt::green);
    QString s("Установлено соединение с сервером " + conn->Name());
    msg->setText(s);
    logger.log(s);
}

// разорвано соединение
void BridgeTcp::disconnected(ClientTcp * conn)
{
    if (conn==mainConnection)
        ui->label_main->set(QLed::round, QLed::on, Qt::yellow);
    else
        ui->label_rsrv->set(QLed::round, QLed::on, Qt::yellow);
    QString s("Разорвано соединение с сервером " + conn->Name());
    msg->setText(s);
    logger.log(s);
}

// ошибка сокета
void BridgeTcp::error       (ClientTcp *conn)
{
    QBrush brush(Qt::yellow);
    QString s = QString("Ошибка подключения %1: %2").arg(conn->Name()).arg(TcpHeader::ErrorInfo(conn->lasterror()));
    msg->setText(s);
    if (conn->lasterror() != QAbstractSocket::ConnectionRefusedError)
    {
        logger.log(s);
        brush = QBrush(Qt::red);
    }
    if (conn==mainConnection)
        ui->label_main->set(QLed::round, QLed::on, brush);
    else
        ui->label_rsrv->set(QLed::round, QLed::on, brush);
}

// готовы форматные данные; необходимо их скопировать, т.к. они будут разрушены
void BridgeTcp::dataready   (ClientTcp * conn)
{
    msg->setText(QString("%1. Получены форматные данные: %2 байт").arg(QTime::currentTime().toString()).arg(conn->RawLength()));
    conn->SendAck();                                      // квитирование
}

// получены необрамленные данные - отдельный сигнал
void BridgeTcp::rawdataready(ClientTcp * conn)
{
    msg->setText(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(conn->RawLength()));
}

