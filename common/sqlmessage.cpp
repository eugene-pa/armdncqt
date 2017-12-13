#include "sqlmessage.h"

//std::queue<SqlMessage*> SqlMessage::Messages;
SqlMessage::SqlMessage(int krug, int st, QString& s, int app, int event, QString ip)
{
    idKrug  = krug;
    idSt    = st;
    host    = ip;
    idApp   = app;
    idEvent = event;
    t       = QDateTime::currentDateTime();
    msg     = s;
//    Messages.push(this);
}

