#include "shapetrain.h"
#include "../spr/train.h"
#include "../spr/rc.h"
#include "shaperc.h"

bool ShapeTrain::bShowTrains        = true;
bool ShapeTrain::bShowNonregTrains  = true;

// "виртуальный" объект ShapeTrain генерится при создании формы последним примитивом
// его функци рисования paint отрисовывает все поезда на РЦ и на слепых перегонах
QString pzd("Поезда");
ShapeTrain::ShapeTrain(ShapeSet* parent) : DShape (pzd, parent)
{
    //int a = 99;
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
    // поезда на РЦ
    qDebug() << "draw trains";
    foreach (Train * train, Train::Trains)
    {
        if (bShowTrains && (bShowNonregTrains || train->no > 0))
        {
            QPointF pLft = QPointF(99999, 99999),                 // крайняя левая точка
                    pRht = QPointF(0, 0);                         // крайняя правая точка

            bool orientLft = true, orientRht = true;
            bool found = false;

            // вылетаем в LINUXE в этой функции - бардак в Rc: nrc не соответствует Rc, Rc.count()==2, но RC[0]=0, Rc[1] !=0
            //
            for (int i=0; i<train->nrc; i++)
            {
                Rc * rc = train->Rc[i];
                if (rc==nullptr)
                    continue;

                qDebug() << "finding...";
                foreach (QGraphicsItem *shape, rc->shapes)
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

                QPointF  * pText;
                QFont font = QFont("Verdana",10, 60);
                painter->setFont(font);
                QString s = train->no ? QString::number(train->no) : QString::number(train->sno);
                QRectF boundRect;

                int flags = Qt::AlignLeft|Qt::AlignTop;
                if ((train->IsEvn() && !orient) || (!train->IsEvn() && orient))
                {
                    pText = &pRht;  //  - new Vector(extrax/2 + text.Width, text.Height + extray + 3);  // базовая точка текста
                    boundRect = painter->boundingRect(pText->x() -2, pText->y()-18,40,24, flags, s) + QMargins(2,0,2,0);  //
                    // ориентация по правому краю выполняем "вручную" смещая надпись влево на ширину номера
                    int w = boundRect.width();
                    boundRect.setLeft(boundRect.x() - w);
                    boundRect.setRight(boundRect.x() + w);
                }
                else
                {
                    pText = &pLft;
                    boundRect = painter->boundingRect(pText->x() + 2, pText->y()-18,40,24, flags, s) + QMargins(2,0,2,0);  //
                }

                qDebug() << "train...";
                painter->fillRect(boundRect, ShapeTrain::GetBrush(train->no));
                boundRect.setLeft(boundRect.x() + 2);
                painter->setPen(train->no ? Qt::white : Qt::black);
                painter->drawText(boundRect, s);

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

QRectF ShapeTrain::boundingRect() const
{
    return QRectF(0,0,1980*2, 1080*2);
}
