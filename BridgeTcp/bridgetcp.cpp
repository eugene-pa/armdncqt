#include "bridgetcp.h"
#include "ui_bridgetcp.h"

Logger logger("Log/shaper.txt", true, true);
QString iniFile = "C:/armdncqt/bridgetcp/bridgetcp.ini";
QString     mainServerConnectStr;                           // ip:порт основного сервера
QString     rsrvServerConnectStr;                           // ip:порт резервного сервера или ""
QHostAddress *ipMain;                                        //
QHostAddress *ipRsrv;

BridgeTcp::BridgeTcp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BridgeTcp)
{
    ui->setupUi(this);

    IniReader rdr(iniFile);
    QString s;
    rdr.GetText("SERVER", s);
    QStringList addrs = s.split(QRegExp("[ ,;]+"));
    mainServerConnectStr = addrs.length() > 0 ? addrs[0] : "";
    rsrvServerConnectStr = addrs.length() > 1 ? addrs[1] : "";

    QString ip = "";
    int port = 0;

    TcpHeader::ParseIpPort(mainServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipMain = mainServerConnectStr.length() ? new QHostAddress(mainServerConnectStr) : nullptr;

    TcpHeader::ParseIpPort(rsrvServerConnectStr, ip, port); // основной сервер разбор строки IP:порт
    ipRsrv = rsrvServerConnectStr.length() ? new QHostAddress(rsrvServerConnectStr) : nullptr;
}

BridgeTcp::~BridgeTcp()
{
    delete ui;
}
