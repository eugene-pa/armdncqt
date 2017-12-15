#ifndef SQLBLACKBOX_H
#define SQLBLACKBOX_H


class SqlBlackBox
{
public:
    SqlBlackBox (QString& mainstr, QString& rsrvstr, Logger *logger);
    //SqlBlackBox (QString mainstr="", QString rsrvstr="", Logger *logger=nullptr);
    //SqlBlackBox (std::vector<QString> connstr, Logger *logger=nullptr);
    ~SqlBlackBox();
private:
    std::vector<class SqlServer*> servers;
};

#endif // SQLBLACKBOX_H
