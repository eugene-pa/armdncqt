#ifndef SQLMESSAGE_H
#define SQLMESSAGE_H

class SqlMessage
{
public:
    SqlMessage(int krug, int st, QString& msg, int app, int event, QString ip);

    QString sql();                                      // получить текст запроса на вставку
private:
    int         idKrug;
    int         idSt;
    QString     host;
    int         idApp;
    int         idEvent;
    QDateTime   t;
    QString     msg;

};

#endif // SQLMESSAGE_H
