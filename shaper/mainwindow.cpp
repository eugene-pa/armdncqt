#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shapechild.h"

#include "../shapes/shapeset.h"
#include "../shapes/shapetrnsp.h"
#include "../shapes/colorscheme.h"
#include "../common/logger.h"

Logger logger("Log/shaper.txt");
QVector<ShapeSet *> sets;                                  // массив форм
ColorScheme * colorScheme;

#ifdef Q_OS_WIN
    QString form ("D:/APO/ArmDnc01/Pictures/Васюринская.shp");      // Табло1
    QString extDb("C:/armdncqt/bd/armext.db");
#endif
#ifdef Q_OS_MAC
    QString form ("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Варилка.shp");
    QString extDb("/Users/evgenyshmelev/armdncqt/bd/armext.db");
#endif
#ifdef Q_OS_LINUX
    QString form ("/Volumes/BOOTCAMP/SKZD/01.Краснодар-Кавказская/Pictures/Варилка.shp";)
    QString extDb("/Users/evgenyshmelev/armdncqt/bd/armext.db");
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //logger.log ("Запуск приложения");

    ui->setupUi(this);

    mdiArea = new QMdiArea;                                             // создаем виджет MDI
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);       // скролбары
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    colorScheme = new ColorScheme(extDb, &logger);
    TrnspDescription::readBd(extDb, logger);
}

MainWindow::~MainWindow()
{
    qDeleteAll(sets.begin(), sets.end());                   // удаляем примитивы
    //logger.log ("Завершение приложения");

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
