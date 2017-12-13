#ifndef SQLMESSAGE_H
#define SQLMESSAGE_H

#include "QDateTime"

class SqlMessage
{
public:
    SqlMessage(int krug, int st, QString& s, int app, int event, QString ip);
    //SqlMessage(int st, QString& ip, int app, int event, QDateTime tmdt, QString& s);
private:
    // static std::queue<SqlMessage*> Messages;
    // static void ThreadConnect(QString );
    // static void ThreadDoSql  (long);

    int         idKrug;
    int         idSt;
    QString     host;
    int         idApp;
    int         idEvent;
    QDateTime   t;
    QString     msg;

};

#endif // SQLMESSAGE_H
