#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H

#include "QString"
#include "QDateTime"
#include "queue"
#include "defines.h"


class SqlBlackBox
{
public:
/*
    enum App
    {
        Rss         = 1,
        Monitor     = 2,
        Tablo       = 3,
        TabloM      = 4,
        Gid         = 5,
        RssMpk      = 6,
        ArmTools    = 7,
        BridgeTcp   = 8,
        GidUral     = 9,
    };

    enum Event
    {
        Alarm       = 1,        // 'Аварийное'
        Tech        = 2,        // 'Технологическое'
        Sys         = 3,        // 'Системное'
        Diag        = 4,        // Диагностическое'
        Programm    = 5,        // Программное'
        Error       = 6,        // Фатальная ошибка'
        Notify      = 7,        // Уведомление'
        Tu          = 8,        // ТУ'
        SuperAlarm  = 9,        // Супераларм'
    };
*/
    SqlBlackBox(int krug, int st, QString& ip, int app, int event, QDateTime tmdt, QString& s);
private:
    static std::queue<SqlBlackBox*> Messages;
    static void ThreadConnect(QString );
    static void ThreadDoSql  (long);

    int         idKrug;
    int         idSt;
    QString     host;
    int         idApp;
    int         idEvent;
    QDateTime   t;
    QString     msg;
};

#endif // SQLBLACKBOX_H
