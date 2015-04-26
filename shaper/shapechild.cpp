#include <QtWidgets>
#include "mainwindow.h"
#include "shapechild.h"

ShapeChild::ShapeChild()
{
    setAttribute(Qt::WA_DeleteOnClose);
}

/*
void ShapeChild::paintEvent(QPaintEvent* )
{
    QPainter p(this);                                                   // Создаём новый объект рисовальщика
    p.setPen(QPen(Qt::red,1,Qt::SolidLine));                            // Настройки рисования
    p.drawLine(0,0,width(),height());                                   // Рисование линии
}
*/

void ShapeChild::closeEvent(QCloseEvent *event)
{
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

