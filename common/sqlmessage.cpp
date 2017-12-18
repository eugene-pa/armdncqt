#include "QDateTime"
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

QString SqlMessage::sql()
{
    return QString("INSERT INTO messages(idst, idkrug, host, app, msgtype, dttm,  message) VALUES (%1, %2, %3, %4, %5, '%6', '%7')").arg(idSt).arg(idKrug).arg(host).arg(idApp).arg(idEvent).arg(t.toString().arg(msg));
}
