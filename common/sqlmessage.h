#ifndef SQLMESSAGE_H
#define SQLMESSAGE_H
#include <QBrush>

class SqlMessage
{
public:
    SqlMessage(int krug, int st, QString& msg, int app, int event, QString ip);// конструктор с явным заданием параметров
    SqlMessage(QSqlQuery q);                            // конструктор по запросу


    QString sql();                                      // получить текст запроса на вставку
    QBrush  GetBackground();                            // получить кисть для фона
//private:
    int         idKrug;
    int         idSt;
    QString     host;
    int         idApp;
    int         idEvent;
    QDateTime   t;
    QDateTime   tsaved;                                 // используется толкьо при чтении
    QString     msg;

};

#endif // SQLMESSAGE_H
