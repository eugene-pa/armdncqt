#include <QCoreApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cout << sizeof(char) << endl;
    cout << sizeof('А')  << endl;
    cout << sizeof('1')  << endl;

    const char * p = "АБВГД";
    cout << strlen(p);

    return a.exec();
}
