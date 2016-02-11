#include <QtWidgets>
#include "mainwindow.h"
#include "shapechild.h"
#include "../shapes/shapeset.h"
#include "../common/logger.h"

ShapeChild::ShapeChild(ShapeSet * set)
{
    shapeSet = set;

    setAttribute(Qt::WA_DeleteOnClose);
    setAlignment(Qt::AlignLeft|Qt::AlignTop);

    scene = new QGraphicsScene(0,0,1920,1080);              //scene->setSceneRect(0,0,1920,1080);
    scene->setBackgroundBrush(Qt::lightGray);

    for (int i=0; i<set->count(); i++)                      // формируем набор графических примитивов сцены
        scene->addItem(set->GetSet()[i]);

    setScene(scene);
    centerOn(0,0);
    //scale(2,2);                                         // масштабирование всего представления

    startTimer(750);
}



void ShapeChild::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
/*
    if (maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
*/
}

void ShapeChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    setWindowModified(false);
}

void ShapeChild::timerEvent(QTimerEvent *event)
{
    DShape::globalPulse = !DShape::globalPulse;
    Q_UNUSED(event)
    scene->update();
}
