#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H


class SqlBlackBox
{
public:
    SqlBlackBox (QString mainstr, QString rsrvstr="", Logger *logger=nullptr);      // конструктор
    ~SqlBlackBox();                                                                 // деструктор
    void putMsg(int krug, int st, QString msg, int app, int event, QString ip="");  // запись сообщения
    void putMsg(int st, QString msg, int app, int event);                           // запись сообщения (перегруженная)

    class SqlServer* GetSvr(int indx);

private:
    std::vector<class SqlServer*> servers;                                      // список серверов
    QString localhost;                                                          // IP адрес локального хоста (используется если не задан IP сообщения)
};

#endif // SQLBLACKBOX_H
