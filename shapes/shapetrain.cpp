#include "shapetrain.h"
#include "../spr/train.h"
#include "../spr/rc.h"
#include "shaperc.h"
//#include "QThread"

bool ShapeTrain::bShowTrains        = true;
bool ShapeTrain::bShowNonregTrains  = false;

// "виртуальный" объект ShapeTrain генерится при создании формы последним примитивом
// его функци рисования paint отрисовывает все поезда на РЦ и на слепых перегонах
QString pzd("Поезда");
ShapeTrain::ShapeTrain(ShapeSet* parent) : DShape (pzd, parent)
{
    //int a = 99;
    type = TRAIN_COD;
}

ShapeTrain::~ShapeTrain()
{

}

void ShapeTrain::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    Draw(painter);
}


// - для каждого поезда просматриваем список РЦ под поездом и определяем крайние левую и правую точки
// - в зависимости от направления поезда и станции отрисовываем номер над крайней левой или правой точкой
void ShapeTrain::Draw(QPainter* painter)
{
    //qDebug() << "Drawing thread: " << QThread::currentThreadId ();
    // поезда на РЦ
    for (auto rec : Train::Trains)
    {
        Train * train = rec.second;
        if (bShowTrains && (bShowNonregTrains || train->no > 0))
        {
            QPointF pLft = QPointF(99999, 99999),                 // крайняя левая точка
                    pRht = QPointF(0, 0);                         // крайняя правая точка

            bool orientLft = true, orientRht = true;
            bool found = false;


            // поиск левой и правой РЦ
            for (int i=0; i<train->nrc; i++)
            {
                Rc * rc = train->Rc[i];
                if (rc==nullptr)
                    continue;

                for (QGraphicsItem *shape : rc->shapes)
                {
                    ShapeRc * shaperc = (ShapeRc *)shape;

                    if (!shaperc->isStrlOk() || shaperc->sprRc==nullptr || !shaperc->sprRc->StsBusy())               // проверка вхождения по стрелкам
                        continue;

                    // проблема: алгоритм не учитывает топологию станций, пример: при отрисовке на схеме станции горловины другой станции
                    // НУ2 предыдущей санции нарисована правее НП2 следующей, поезд занимающий обе РЦ будет отрисован на самой правой (НУ2),
                    // а правильнее на НП2
                    found = true;
                    if (pLft.x() > shaperc->x1)             // проверяю оба конца отрезка на случай перевернутых отрезков
                    {
                        pLft = shaperc->xy();               // X1 левее pLft
                        orientLft = shaperc->St()->IsOrientEvnOdd();
                    }
                    if (pLft.x() > shaperc->x2)             // X2 левее pLft
                    {
                        pLft = shaperc->x2y2();
                        orientLft = shaperc->St()->IsOrientEvnOdd();
                    }
                    if (pRht.x() < shaperc->x1)
                    {
                        pRht = shaperc->xy();               // X1 правее pRht
                        orientRht = !shaperc->St()->IsOrientEvnOdd(); // 2015.08.21. инверсия
                    }
                    if (pRht.x() < shaperc->x2)             // X2 правее pRht
                    {
                        pRht = shaperc->x2y2();
                        orientRht = !shaperc->St()->IsOrientEvnOdd(); // 2015.08.21. инверсия
                    }
                }
            }

            if (found)
            {
                bool orient = orientLft == orientRht ? orientLft : orientRht;
                drawTrain(painter, train, orient, pLft, pRht);  // отрисовка поезда
            }
        }
    }

    // поезда на слепых перегонах (наприсовать!)
}

QBrush ShapeTrain::GetBrush(int no)
{
    return  no ==0      ?	Qt::yellow      :
            no < 1000   ?	Qt::red         :
            no >=9800	?	Qt::lightGray   :
            no > 6000	?	Qt::darkRed     :
            no > 4000	?	Qt::darkGreen   :
                            Qt::darkBlue    ;
}


// решаем проблему глюка с отрисовкой номеров поездов: причина пропадания заключалась в выпадании области единственного примитива,
// используемого для отрисовки всех поездов из области просмотра, так как функция boundingRect(), определенная в shape
// не была переопределена
// Здесь функция возвращает область из расчета 4-х мониторов (2*2) в режиме full hd
QRectF ShapeTrain::boundingRect() const
{
    return QRectF(0,0,1980*2, 1080*2);
}

void ShapeTrain::drawTrain(QPainter* painter, class Train*train, bool orient, QPointF pLft, QPointF pRht)
{
#ifdef Q_OS_WIN
                int fsize = 10;
#endif
#ifdef Q_OS_MAC
                int fsize = 12;
#endif
#ifdef Q_OS_LINUX
                int fsize = 10;
#endif
    QFont font = QFont("Verdana",fsize, 60);
    painter->setFont(font);
    QString s = train->no ? QString::number(train->no) : QString::number(train->sno);
    QRectF boundRect;
    QPolygonF header;
    QPointF  * pText;

    int flags = Qt::AlignLeft|Qt::AlignTop;
    if ((train->IsEvn() && !orient) || (!train->IsEvn() && orient))
    {
        // едем вправо
        pText = &pRht;  //  - new Vector(extrax/2 + text.Width, text.Height + extray + 3);  // базовая точка текста
        boundRect = painter->boundingRect(pText->x() -2, pText->y()-18,40,24, flags, s) + QMargins(2,0,2,0);  //
        // ориентацию по правому краю выполняем "вручную" смещая надпись влево на ширину номера
        int w = boundRect.width();
        boundRect.setLeft(boundRect.x() - w);
        boundRect.setRight(boundRect.x() + w);

        // Формируем полигон направления
        header << boundRect.topRight();
        header << boundRect.bottomRight();
        header << (boundRect.topRight() + QPointF(7,boundRect.height()/2));
        header << boundRect.topRight();
    }
    else
    {
        // едем влево
        pText = &pLft;
        boundRect = painter->boundingRect(pText->x() + 8, pText->y()-18,40,24, flags, s) + QMargins(2,0,2,0);  //

        // Формируем полигон направления
        header << boundRect.topLeft();
        header << boundRect.bottomLeft();
        header << (boundRect.topLeft() + QPointF(-7,boundRect.height()/2));
        header << boundRect.topLeft();
    }

    painter->setBrush(ShapeTrain::GetBrush(train->no));
    painter->setPen(Qt::transparent/*ShapeTrain::GetBrush(train->no).color()*/);
    painter->drawPolygon(header);

    painter->fillRect(boundRect, ShapeTrain::GetBrush(train->no));
    boundRect.setLeft(boundRect.x() + 2);
    painter->setPen(train->no ? Qt::white : Qt::black);
    painter->drawText(boundRect, s);

}
