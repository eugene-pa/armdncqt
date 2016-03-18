#ifndef SHAPECHILD_H
#define SHAPECHILD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "mainwindow.h"

class ShapeChild : public QGraphicsView
{
    Q_OBJECT

public:
    ShapeChild(class ShapeSet * set);
    QString currentFile() { return curFile; }

    void changeShapeSet(class ShapeSet * set);              // смена формы
    void clear() { scene->clear(); }

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private slots:

private:
    QString curFile;

    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName)
    {
	return QFileInfo(fullFileName).fileName();    
    }
    QString userFriendlyCurrentFile() { return strippedName(curFile); }

    QGraphicsScene * scene;
    class ShapeSet * shapeSet;
};

#endif
