#include <QCoreApplication>
#include <../common/rsasinc.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RsAsinc rs ("COM3,38400,N,8,1");
    QByteArray data("It's me!");
    rs.Send(data);

    return a.exec();
}
