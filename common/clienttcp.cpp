#include "clienttcp.h"

ClientTcp::ClientTcp(QString ipport)
{

}


ClientTcp::ClientTcp(QString ip, int port)
{
    this->ip = ip;
    this->port = port;
}

ClientTcp::~ClientTcp()
{
    sock->abort();
    sock->close();
    delete data;
}

void ClientTcp::init()
{
    compress = false;
    sock = new QTcpSocket();
    data = new char[65536 + 8];
}
