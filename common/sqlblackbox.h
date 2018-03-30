#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H


class SqlBlackBox
{
public:
    static std::unordered_map <int, QString  > appNames;                        // имена приложений, индексированные по ID
    static std::unordered_map <int, QString  > msgTypes;                        // имена типов сообщений, индексированные по ID
    static int GetAppIdByName (QString s);                                      // получить ключ приложения по имени или 0
    static int GetMsgTypeByName(QString s);                                     // получить ключ типа сообщения по имени или 0

    void load();                                                                // загрузить cправочники приложений и типов сообщений

    SqlBlackBox (QString mainstr, QString rsrvstr="", Logger *logger=nullptr);      // конструктор
    ~SqlBlackBox();                                                                 // деструктор
    void putMsg(int krug, int st, QString msg, int app, int event, QString ip="");  // запись сообщения
    void putMsg(int st, QString msg, int app, int event);                           // запись сообщения (перегруженная)
    void putMsg(QString msg, QString app, QString event, QString ip, int st,int krug=0);  // запись сообщения с текстовыми параметрами

    class SqlServer* GetSvr(int indx);

private:
    std::vector<class SqlServer*> servers;                                      // список серверов
    QString localhost;                                                          // IP адрес локального хоста (используется если не задан IP сообщения)
};



#endif // SQLBLACKBOX_H
