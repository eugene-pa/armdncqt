#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

#include "shapeset.h"
#include "shaperc.h"
#include "shapestrl.h"
#include "shapesvtf.h"
#include "shapetrnsp.h"
#include "shapetext.h"
#include "shapeprzd.h"
#include "shapebox.h"
#include "shapetrain.h"

float ShapeSet::X0 = 0.0,
      ShapeSet::Y0 = 0.0;										// общее смещение схем, задавыаемое опцией X0=, Н0=

ShapeSet::ShapeSet(QString path, Logger * logger, bool bTablo/*=false*/)
{
    plog = logger;
    bTabloShape = bTablo;
    xMin = yMin = 999999.;									// Минимальная ккордината примитива
    xMax = yMax = -999999.;									// Максимальная ккордината примитива по оси X

    filePath = path;                                        // путь к файлу формы
    formname = QFileInfo(path).baseName();                  // имя формы (основа имени файла без расширения)
    compactSvtf = true;
    nDiameter	= 0;										// индивидуальная установка диаметра светофора
    nThick		= 0;										// индивидуальная установка толщины линий

//    if (DStation::sShpTablo == Name)
//    {
//        nDiameter	= nTabloDiameter;						// индивидуальная установка диаметра светофора
//        nThick		= nTabloThick;							// индивидуальная установка толщины линий
//    }

    logger->log (QString("Загрузка: %1").arg(path));
    ScanFile();												// Сканировать файл

    if (!bTabloShape && (X0 || Y0))
        MoveRel(X0,Y0);
}

ShapeSet::~ShapeSet()
{
    qDebug() << "Деструктор ShapeSet: " << formname;
    qDeleteAll(set.begin(), set.end());                     // удаляем примитивы
    set.clear();                                            // очищаем список
    //delete set;                                           // удаляем список
}

// Сканирование/загрузка файла
void ShapeSet::ScanFile()
{
    QFile file (filePath);
    if (file.open(QFile::ReadOnly))
    {
        QTextStream in (&file);
        in.setCodec("Windows-1251");
        QString str;

        while (!(str = in.readLine()).isNull())
        {
            DShape * shape = 0;
            QStringList lexems = str.trimmed().split(' ');
            if (lexems.count())
            {
                int type = lexems[0].toInt();
                switch (type)
                {
                    case BOX_COD :      shape = new ShapeBox  (str,this);   /*setText .append(shape);*/ break;
                    case SEGMENT_COD:
                    case ANGLE_COD:     shape = new ShapeRc   (str,this);   setRc   .append(shape); break;
                    case STRL_COD:      shape = new ShapeStrl (str,this);   setStrl .append(shape); break;
                    case SVTF_COD:      shape = new ShapeSvtf (str,this);   setSvtf .append(shape); break;
                    case MODE_COD:      shape = new ShapeTrnsp(str,this);   setTrnsp.append(shape); break;
                    case PRZD_COD:      shape = new ShapePrzd (str,this);   /*setPrzd .append(shape);*/ break;
                    case NAME_COD:      shape = new ShapeText (str,this);   /*setName .append(shape);*/ break;
                    case TEXT_COD:      shape = new ShapeText (str,this);   /*setText .append(shape);*/ break;
                }
                if (shape != 0)
                {
                    set.append(shape);
                }
            }
        }

        // проблема: номера поездов не отрисовываются при масштабе > 1:1
        set.append(new ShapeTrain(this));
        //set.append(new ShapeTrain(this));
    }
    else
    {
        plog->log(QString("Ошибка доступа к файлу %1").arg(filePath));
    }
}

// перемещение на заданное расстояние
void ShapeSet::MoveRel (float x0, float y0)
{
    Q_UNUSED(x0)
    Q_UNUSED(y0)
}


// чтение форм
void ShapeSet::ReadShapes(QString dir, Logger * logger)
{
    foreach (Station * st, Station::Stations)
    {
        foreach (ShapeId * p, st->formList)
        {
            p->set = new ShapeSet(dir + p->fileName, logger);
        }
    }
}

// активация формы
// - очистка списков примитивов РЦ
// - заполнение спсиска примитивов РЦ
void ShapeSet::Activate()
{
    Rc::ClearShapes();
    foreach (DShape * shape, setRc)
    {
        ((ShapeRc *)shape)->AddAndMerge();
    }
}
