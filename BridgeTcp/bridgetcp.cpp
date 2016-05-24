#include "bridgetcp.h"
#include "ui_bridgetcp.h"

Logger logger("Log/shaper.txt", true, true);                // лог

#ifdef Q_OS_WIN
QString iniFile = "c:/armdncqt/bridgetcp/bridgetcp.ini";    // настройки
QString images(":/status/images/");                         // путь к образам
#endif
#ifdef Q_OS_MAC
QString iniFile = "/Users/evgenyshmelev/armdncqt/bridgetcp/bridgetcp.ini";  // настройки
QString images("/Users/evgenyshmelev/armdncqt/images/");    // путь к образам
#endif
#ifdef Q_OS_LINUX
QString iniFile = "/home/eugene/QTProjects//bridgetcp/bridgetcp.ini";       // настройки
QString images("../images/");
#endif

bool compressEnabled = true;                                // сжатие на летк
QString     mainServerConnectStr;                           // ip:порт основного сервера
QString     rsrvServerConnectStr;                           // ip:порт резервного сервера или ""
QHostAddress *ipMain;                                       //
QHostAddress *ipRsrv;

int      portBridge = 1010;                                 // порт шлюза

BridgeTcp::BridgeTcp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BridgeTcp)
{
    ui->setupUi(this);

    loadResources();

    bar = new QStatusBar(this);
    ui->horizontalLayoutBar->addWidget(bar);
    bar->addWidget(msg = new QLabel("123"));
    msg->setFrameStyle(QFrame::NoFrame);


    IniReader rdr(iniFile);
    QString s;
    rdr.GetInt("BRIDGEPORT", portBridge);
    ui->lineEdit_BridgePort->setText(QString::number(portBridge));

    rdr.GetText("SERVER", s);
    QStringList addrs = s.split(QRegExp("[ ,;]+"));
    ui->lineEdit_main->setText(mainServerConnectStr = addrs.length() > 0 ? addrs[0] : "");
    ui->lineEdit_rsrv->setText(rsrvServerConnectStr = addrs.length() > 1 ? addrs[1] : "");

    rdr.GetBool("COMPRESSDATA",compressEnabled);
    ui->checkBox->setChecked(compressEnabled);

    QString ip = "";
    int port = 0;

    TcpHeader::ParseIpPort(mainServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipMain = mainServerConnectStr.length() ? new QHostAddress(mainServerConnectStr) : nullptr;
    mainConnection = new ClientTcp(mainServerConnectStr, &logger, compressEnabled, "Шлюз СПД");
    connections.append(mainConnection);

    QObject::connect(mainConnection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
    QObject::connect(mainConnection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
    ui->label_main->set(QLed::round, QLed::on, Qt::yellow);

    TcpHeader::ParseIpPort(rsrvServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipRsrv = rsrvServerConnectStr.length() ? new QHostAddress(rsrvServerConnectStr) : nullptr;
    if (ipRsrv != nullptr)
    {
        rsrvConnection = new ClientTcp(rsrvServerConnectStr, &logger, compressEnabled, "Шлюз СПД");
        connections.append(rsrvConnection);
        QObject::connect(rsrvConnection, SIGNAL(connected   (ClientTcp*)), this, SLOT(connected   (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(disconnected(ClientTcp*)), this, SLOT(disconnected(ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(error       (ClientTcp*)), this, SLOT(error       (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(dataready   (ClientTcp*)), this, SLOT(dataready   (ClientTcp*)));
        QObject::connect(rsrvConnection, SIGNAL(rawdataready(ClientTcp*)), this, SLOT(rawdataready(ClientTcp*)));
        ui->label_rsrv->set(QLed::round, QLed::on, Qt::yellow);
    }
    else
        ui->label_rsrv->set(QLed::round, QLed::off);

    QTableWidget * t = ui->tableWidget;
    t->setColumnCount(4);
    t->verticalHeader()->setDefaultSectionSize(20);
    t->setColumnWidth(0,150);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setHorizontalHeaderLabels(QStringList() << "Хост" << "Тип" << "Передано" << "Принято");
    t->setSortingEnabled(false);
    t->setRowCount(connections.count());
    for (int i=0; i<connections.count(); i++)
    {
        ClientTcp *conn = connections[i];
        QTableWidgetItem * item = new QTableWidgetItem (conn->name());
        t->setItem(i,0, item);
        t->setItem(i,1, new QTableWidgetItem ("Сервер"));
        t->item(i,0)->setData(Qt::UserRole,qVariantFromValue((void *)conn));    // запомним клиента
    }

    // старт подключений после заполнения таблицы
    for (int i=0; i<connections.count(); i++)
    {
        connections[i]->start();
    }

    server = new ServerTcp(portBridge, QHostAddress::Any, &logger);
    QObject::connect(server, SIGNAL(acceptError(ClientTcp*)), this, SLOT(slotAcceptError(ClientTcp*)));
    QObject::connect(server, SIGNAL(newConnection(ClientTcp*)), this, SLOT(slotSvrNewConnection(ClientTcp*)));
    QObject::connect(server, SIGNAL(dataready(ClientTcp*)), this, SLOT(slotSvrDataready(ClientTcp*)));
    QObject::connect(server, SIGNAL(disconnected(ClientTcp*)), this, SLOT(slotSvrDisconnected(ClientTcp*)));

    startTimer(1000);
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
    QString s("Установлено соединение с сервером " + conn->name());
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
    QString s("Разорвано соединение с сервером " + conn->name());
    msg->setText(s);
    logger.log(s);
}

// ошибка сокета
void BridgeTcp::error       (ClientTcp *conn)
{
    QBrush brush(Qt::yellow);
    QString s = QString("Ошибка подключения %1: %2").arg(conn->name()).arg(TcpHeader::ErrorInfo(conn->lasterror()));
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
    msg->setText(QString("%1. Ретранслируем данные от сервера %2: %3 байт").arg(QTime::currentTime().toString()).arg(conn->name()).arg(conn->rawLength()));
    conn->sendAck();                                      // квитирование

    if (server != nullptr)
    {
        // если сжатие разрешено и данные уже сжаты, отправляем их без доп.обработки
        if (compressEnabled && conn->isCompressed())
            server->sendToAll(conn->rawData(), conn->rawLength());
        else
            server->packsendToAll(conn->data(), conn->length(), compressEnabled);
    }
}

// получены необрамленные данные - отдельный сигнал
void BridgeTcp::rawdataready(ClientTcp * conn)
{
    msg->setText(QString("%1. Получены неформатные данные: %2 байт").arg(QTime::currentTime().toString()).arg(conn->rawLength()));
}



// уведомления сервера
// ошибка на сокете
void BridgeTcp::slotAcceptError(QAbstractSocket::SocketError socketError)
{
}

// подключен новый клиент
void BridgeTcp::slotSvrNewConnection (ClientTcp *conn)
{
    QString s("Подключен клиент " + conn->name());
    msg->setText(s);
    logger.log(s);

    QTableWidget * t = ui->tableWidget;
    t->setRowCount(connections.count() + server->clients().count());
    int row = t->rowCount()-1;
    QString name = conn->name();
    t->setItem(row,0, new QTableWidgetItem (conn->name()));
    t->setItem(row,1, new QTableWidgetItem (conn->getid()));
    t->setItem(row,2, new QTableWidgetItem (QString("%1/%2").arg(conn->getsent(0)).arg(conn->getsent(1))));
    t->setItem(row,3, new QTableWidgetItem (QString("%1/%2").arg(conn->getrcvd(0)).arg(conn->getrcvd(1))));
    t->item(row,0)->setData(Qt::UserRole,qVariantFromValue((void *)conn));    // запомним клиента
}

void BridgeTcp::slotSvrDisconnected  (ClientTcp * conn)
{
    QString s("Отключен клиент " + conn->name());
    msg->setText(s);
    logger.log(s);
}

// получены данные
void BridgeTcp::slotSvrDataready     (ClientTcp * conn)
{
    QString name(conn->name());
    QString s("Приняты данные от клиента " + conn->name());
    msg->setText(s);
}

// таймер
void BridgeTcp::timerEvent(QTimerEvent *event)
{
    QTableWidget * t = ui->tableWidget;
    for (int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem * item = ui->tableWidget->item(i,0);
        if (item != nullptr)
        {
            ClientTcp *conn = (ClientTcp *) ui->tableWidget->item(i,0)->data(Qt::UserRole).value<void*>();

            if (conn != nullptr)
            {
                if (i >= connections.count())
                    t->setItem(i,1, new QTableWidgetItem (conn->getid()));
                QIcon icon = QIcon(conn->isConnected() ? *g_green : *g_yellow);
                t->item(i,0)->setIcon(QIcon(conn->isConnected() ? *g_green : *g_yellow));
                t->setItem(i,2, new QTableWidgetItem (QString("%1/%2").arg(conn->getsent(0)).arg(conn->getsent(1))));
                t->setItem(i,3, new QTableWidgetItem (QString("%1/%2").arg(conn->getrcvd(0)).arg(conn->getrcvd(1))));

                // отключенные клиенты удаляем из списка; удаляем объект ClientTcp
                if (i >= connections.count() && !conn->isConnected())
                {
                    t->removeRow(i);
                    delete conn;
                    break;
                }
            }
        }
    }
}

void BridgeTcp::loadResources()
{
    g_green             = new QPixmap(images + "icon_grn.ico");
    g_red               = new QPixmap(images + "icon_red.ico");
    g_yellow            = new QPixmap(images + "icon_yel.ico");
    g_gray              = new QPixmap(images + "icon_gry.ico");
    g_white             = new QPixmap(images + "icon_wht.ico");
    g_cyan              = new QPixmap(images + "icon_cyn.ico");

    g_green_box_blink   = new QPixmap(images + "box_grn_blink.ico");
    g_green_box         = new QPixmap(images + "box_grn.ico");
    g_green_box_tu      = new QPixmap(images + "box_grn_tu.ico");           // МТУ ок
    g_green_dark_box    = new QPixmap(images + "box_grn_dark.ico");
    g_red_box           = new QPixmap(images + "box_red.ico");
    g_red_box_tu        = new QPixmap(images + "box_red_tu.ico");           // МТУ error
    g_red_dark_box      = new QPixmap(images + "box_red_dark.ico");
    g_yellow_box        = new QPixmap(images + "box_yel.ico");
    g_yellow_dark_box   = new QPixmap(images + "box_yel_dark.ico");
    g_gray_box          = new QPixmap(images + "box_gry.ico");
    g_white_box         = new QPixmap(images + "box_wht.ico");

    g_strl_minus        = new QPixmap(images + "strl_minus.ico");           // -
    g_strl_plus         = new QPixmap(images + "strl_plus.ico");            // +

}
