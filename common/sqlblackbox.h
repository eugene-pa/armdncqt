#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H


class SqlBlackBox
{
public:
    SqlBlackBox (QString mainstr, QString rsrvstr="", Logger *logger=nullptr);
    ~SqlBlackBox();
    void putMsg(int krug, int st, QString msg, int app, int event, QString ip); // запись сообщения
    void putMsg(int st, QString msg, int app, int event);                       // запись сообщения (перегруженная)

private:
    std::vector<class SqlServer*> servers;
    QString localhost;

};

#endif // SQLBLACKBOX_H
