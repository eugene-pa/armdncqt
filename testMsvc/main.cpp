#include <QCoreApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    using namespace std;
    cout << sizeof(char) << ",";
    cout << sizeof('А') << ",";
    cout << sizeof('1') << ",";

    const char * p = "АБВГД";
    cout << strlen(p);

    return a.exec();
}
