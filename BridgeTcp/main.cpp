#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //BridgeTcp w;
    MainWindow w(nullptr);
    w.show();


    return a.exec();
}
