#ifndef SHAPECHILD_H
#define SHAPECHILD_H

#include "mainwindow.h"

class ShapeChild : public QWidget
{
    Q_OBJECT

public:
    ShapeChild();
    QString currentFile() { return curFile; }

protected:
    void paintEvent(QPaintEvent* pe);
    void closeEvent(QCloseEvent *event);

private slots:

private:
    QString curFile;

    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName)
    {
	return QFileInfo(fullFileName).fileName();    
    }
    QString userFriendlyCurrentFile() { return strippedName(curFile); }

};

#endif
