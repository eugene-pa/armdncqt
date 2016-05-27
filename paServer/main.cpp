#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QString s = QHostInfo::localHostName();
    QHostInfo host = QHostInfo::fromName(s);
    QList<QHostAddress> list = host.addresses();

    w.show();

    return a.exec();
}
