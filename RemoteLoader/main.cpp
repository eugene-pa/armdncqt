#include "dialog.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QStringList>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // локализация стандпртных диалогов
    QTranslator qtTranslator;
    qtTranslator.load("qt_ru",QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    if (argc < 3 || argc> 5)
    {
        QMessageBox::information(nullptr, "RemoteLoader. Неверное задание параметров", "Порядок использования утилиты:\nRemoteLoader IPaddress [*]SrcFileOrDirectory[*.ext] DstFileOrDirectory [/u [/y]]" );
        return -1;
    }

    QStringList list;
    for (int i=1; i<argc; i++)
        list.append(argv[i]);

    Dialog w(list);
    w.show();

    return a.exec();
}

