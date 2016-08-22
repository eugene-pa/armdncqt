#include <QtWidgets>
#include "mainwindow.h"
#include "shapechild.h"
#include "../shapes/shapeset.h"
#include "../common/logger.h"

ShapeChild::ShapeChild(ShapeSet * set)
{
    shapeSet = set;
    set->Activate();

    setAttribute(Qt::WA_DeleteOnClose);
    setAlignment(Qt::AlignLeft|Qt::AlignTop);

    scene = new QGraphicsScene(0,0,1920*2,1080*2);              //scene->setSceneRect(0,0,1920,1080);
    scene->setBackgroundBrush(QColor(211,211,211));         // Qt::lightGray

    for (int i=0; i<set->count(); i++)                      // формируем набор графических примитивов сцены
        scene->addItem(set->GetSet()[i]);

    setScene(scene);
    centerOn(0,0);
    //scale(2,2);                                         // масштабирование всего представления

    startTimer(750);

    setMouseTracking(true);

#ifdef Q_OS_WIN
    tooltipFont = QFont("Arial",12);
#endif
#ifdef Q_OS_MAC
    tooltipFont = QFont("Arial",14);
#endif
#ifdef Q_OS_WIN
    tooltipFont = QFont("Arial",12);
#endif

    QToolTip::setFont(tooltipFont);
}

// смена формы
void ShapeChild::changeShapeSet(class ShapeSet * set)
{
    shapeSet = set;
    set->Activate();

    scene->clear();
    for (int i=0; i<set->count(); i++)                      // формируем набор графических примитивов сцены
        scene->addItem(set->GetSet()[i]);
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
    DShape::globalPulse = !DShape::globalPulse;             // в случае нескольких окон это делать нельзя, так как переменная DShape::globalPulse одна
    Q_UNUSED(event)
    scene->update();

    // пробовал принудительно отрисовывать поезда при масштабировании - бесполезно!
//    QPainter p;
//    p.begin(this);
//    shapeSet->GetSet()[shapeSet->GetSet().count()-1]->Draw(&p);
//    p.end();
}
/*
bool ShapeChild::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        //int index = itemAt(helpEvent->pos());
        DShape * shape = shapeSet->GetNearestShape(helpEvent->pos());
        if (shape != nullptr)
            QToolTip::showText(helpEvent->globalPos(), shape->Dump());
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
    }
    return QWidget::event(event);
}
*/

// отображаем тултип при перемещении мыши по мере необходимости
// КЛЮЧЕВОЙ МОМЕНТ: учет масштабирования и скола схемы выполняется с помощью ИНВЕРТИРОВННОЙ матрицы трансформации ВЬЮПОРТА:
//      QTransform t = viewportTransform().inverted();
//      QPoint point = t.map(p);
void ShapeChild::mouseMoveEvent(QMouseEvent *event)
{
    QPoint  p = event->pos();
    QTransform t = viewportTransform().inverted();
    QPoint point = t.map(p);

    DShape * shape = shapeSet->GetNearestShape(point);
    if (shape != nullptr)
        QToolTip::showText(event->globalPos(), shape->Dump());
    else
    {
        QToolTip::hideText();
    }
}
