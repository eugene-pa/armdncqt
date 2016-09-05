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
}

// отображаем тултип при перемещении мыши по мере необходимости
// КЛЮЧЕВОЙ МОМЕНТ: учет масштабирования и скролла схемы выполняется с помощью ИНВЕРТИРОВННОЙ матрицы трансформации ВЬЮПОРТА:
//      QTransform t = viewportTransform().inverted();
//      QPoint point = t.map(p);
std::vector<ShapeType> tooltipShapes = { SEGMENT_COD, ANGLE_COD, SVTF_COD, STRL_COD, MODE_COD };
void ShapeChild::mouseMoveEvent(QMouseEvent *event)
{
    QPoint  p = event->pos();
    QTransform t = viewportTransform().inverted();
    QPoint point = t.map(p);

    // если нужно выводить тултипы на тексте, можно сделать 2 итерации - без текста, затем отдельно текст
    // иначе часто попадаем на текст ДО ОБЪЕКТОВ, особенно в компактных схемах
    DShape * shape = shapeSet->GetNearestShape(point, &tooltipShapes);
    if (shape != nullptr)
        QToolTip::showText(event->globalPos(), shape->Dump());
    else
    {
        QToolTip::hideText();
    }
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
