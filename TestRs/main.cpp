#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef Q_OS_WIN
    // настраиваем консоль Windows на юникод
    // под LINUX строки юникод отображаются на консоли нормально без всяких танцев!
//    _setmode(_fileno(stdout), _O_U16TEXT);
//    _setmode(_fileno(stdin),  _O_U16TEXT);
//    _setmode(_fileno(stderr), _O_U16TEXT);
#endif  // #ifdef Q_OS_WIN

    Dialog w;
    w.show();

    int ret = a.exec();
    qDebug() << "emd main";
    return ret;
}
