#ifndef SHAPESET_H
#define SHAPESET_H

#include "shape.h"
#include "../common/logger.h"

extern void Log (QString msg);

class ShapeSet
{
    friend class DShape;
    friend class ShapeRc;
    friend class ShapeSvtf;
    friend class ShapeStrl;

protected:
    QString filePath;					    // полный пкть к файлу формы
    QString formname;                                       // имя
    QVector<class DShape *> set;                            // массив примитивов
    QVector<class DShape *> setRc;                          // массив отрезков
    QVector<class DShape *> setStrl;                        // массив стрелок
    QVector<class DShape *> setSvtf;                        // массив светофоров
    QVector<class DShape *> setTrnsp;                       // массив транспарантов
    QVector<class DShape *> setPrzd;                        // массив переездов

    bool   bTabloShape;                                     // Обзорный кадр
    float  xMin;					    // Минимальная ккордината примитива по оси X
    float  yMin;					    // Минимальная ккордината примитива по оси Y
    float  xMax;					    // Максимальная ккордината примитива по оси X
    float  yMax;					    // Максимальная ккордината примитива по оси Y

    bool   compactSvtf;                                     // компактное отображение светофоров
    int	   nDiameter;					    // индивидуальная установка диаметра светофора для формы
    int	   nThick;					    // индивидуальная установка толщины линий
    Logger * plog;

    void ScanFile();                                        // Сканирование/загрузка файла
public:
    ShapeSet(QString path, Logger * logger, bool bTablo=false);
    ~ShapeSet();

    static void ReadShapes(QString dir, Logger * logger);                // чтение форм

    int  Show (QPainter * painter, bool bBlinking=false);   // Вывод видеоформы
    void Activate();

    void MoveRel (float,float);				    // перемещение на заданное расстояние
    void Prepare ();

    DShape * GetNearestShape(QPoint Pnt, ShapeType type=END_COD);
    DShape * GetNearestShape(QPoint Pnt, ShapeType *type, int n);

    float   GetxMin() { return xMin; }			    // Минимальная координата примитива по оси X
    float   GetyMin() { return yMin; }		            // Минимальная координата примитива по оси Y
    float   GetxMax() { return xMax; }		            // Максимальная координата примитива по оси X
    float   GetyMax() { return yMax; }		            // Максимальная координата примитива по оси Y

    inline QVector<DShape *>  GetSet() { return set; }
    inline QString GetFileName() { return filePath;	}   // путь к файлу
    inline QString Name	      () { return formname; }   // имя без пути и расширения
    inline int count          () { return set.size();	}           // всего примитивов
    inline bool IsTabloShape  () { return bTabloShape;}
    inline Logger * logger    () { return plog; }               // базовый логгер
    static float X0,Y0;					    // общее смещение схем, задавыаемое опцией X0=, Н0=

    //static int nTabloDiameter;			    // опционируемая установка диаметра светофора ТАБЛО
    //static int nTabloThick;				    // опционируемая установка толщины линий ТАБЛО
    //static int nDefaultDiameter;			    // установка диаметра светофора по умолчанию
    //static int nDefaultThick;				    // установка толщины линий по умолчанию
};

#endif // SHAPESET_H
