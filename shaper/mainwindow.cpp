#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"

#include "../shapes/shapeset.h"
#include "../shapes/shapetrnsp.h"
#include "../shapes/colorscheme.h"
#include "../common/logger.h"
#include "../spr/station.h"
#include "../spr/ts.h"
#include "../spr/properties.h"


Logger logger("Log/shaper.txt", true, true);
QVector<ShapeSet *> sets;                                           // массив форм
ColorScheme * colorScheme;

#ifdef Q_OS_WIN
    QString dbname("C:/armdncqt/bd/arm.db");
    QString extDb ("C:/armdncqt/bd/armext.db");
    QString form  ("C:/armdncqt/pictures/Назаровский.shp");         // Табло1
#endif
#ifdef Q_OS_MAC
    QString dbname("/Users/evgenyshmelev/armdncqt/bd/arm.db");
    QString extDb ("/Users/evgenyshmelev/armdncqt/bd/armext.db");
    QString form  ("/Users/evgenyshmelev/armdncqt/Pictures/Назаровский.shp");
#endif
#ifdef Q_OS_LINUX
    QString dbname("/home/eugene/QTProjects/armdncqt/bd/arm.db");
    QString extDb ("/home/eugene/QTProjects/armdncqt/bd/armext.db");
    QString form  ("/home/eugene/QTProjects/armdncqt/pictures/Назаровский.shp");
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    logger.log ("Запуск приложения");

    ui->setupUi(this);

    mdiArea = new QMdiArea;                                             // создаем виджет MDI
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);       // скролбары
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    Station::ReadBd(dbname, logger);
    IdentityType::ReadBd (extDb, logger);
    Ts::ReadBd(dbname, logger);


    colorScheme = new ColorScheme(extDb, &logger);
    TrnspDescription::readBd(extDb, logger);

}

MainWindow::~MainWindow()
{
    logger.log ("Завершение приложения");
    qDeleteAll(sets.begin(), sets.end());                               // удаляем примитивы
    delete ui;
}

void Log (QString msg)
{
    logger.log(msg);
}


// новая форма
void MainWindow::on_actionNewForm_triggered()
{
    ShapeChild * child = new ShapeChild(new ShapeSet(form, &logger));
    mdiArea->addSubWindow(child);
    child->show();
    //ShapeChild
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Варилка.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Васюринская.shp"));
    //sets.append(new ShapeSet("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Гетмановская.shp"));
}
