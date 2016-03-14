#include "shapetrain.h"
#include "../spr/train.h"
#include "../spr/rc.h"
#include "shaperc.h"

bool ShapeTrain::bShowTrains        = true;
bool ShapeTrain::bShowNonregTrains  = true;

ShapeTrain::ShapeTrain(ShapeSet* parent) : DShape (QString("Поезда"), parent)
{
    int a = 99;
}

ShapeTrain::~ShapeTrain()
{

}

void ShapeTrain::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Draw(painter);
}

void ShapeTrain::Draw(QPainter* painter)
{
    foreach (Train * train, Train::Trains)
    {
        if (bShowTrains && (bShowNonregTrains || train->no > 0))
        {
            QPointF pLft = QPointF(99999, 99999),                 // крайняя левая точка
                    pRht = QPointF(0, 0);                         // крайняя правая точка

            bool orientLft = true, orientRht = true;
            bool found = false;

            for (int i=0; i<train->nrc; i++)
            {
                Rc * rc = train->Rc[i];
                foreach (QGraphicsItem *shape, rc->shapes)
                {
//                    if (!(shape is ShapeRc))
//                        continue; // нонсенс
                    ShapeRc * shaperc = (ShapeRc *)shape;
//                    ShapeRc shapeRc = shape as ShapeRc;
//                    if (shapeRc.SprRc == null || shapeRc.SprRc.SprSt == null)
//                        continue; // пропускаем неопределенные

                    if (!shaperc->isStrlOk() || shaperc->sprRc==nullptr || !shaperc->sprRc->StsBusy())               // проверка вхождения по стрелкам
                        continue;

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
                QFont font = QFont("Consolas",12, 60);
                painter->setFont(font);
                QString s = train->no ? QString::number(train->no) : QString::number(train->sno);
                QRectF boundRect;

                int flags = Qt::AlignLeft|Qt::AlignTop;
                if ((train->IsEvn() && !orient) || (!train->IsEvn() && orient))
                {
                    //flags = Qt::AlignRight|Qt::AlignTop;
                    pText = &pRht;  //  - new Vector(extrax/2 + text.Width, text.Height + extray + 3);  // базовая точка текста
                    boundRect = painter->boundingRect(pText->x(), pText->y()-22,40,24, flags, s) + QMargins(2,0,2,0);  //
                    int w = boundRect.width();
                    boundRect.setLeft(boundRect.x() - w);
                    boundRect.setRight(boundRect.x() + w);
                }
                else
                {
                    //Qt::AlignLeft|Qt::AlignTop;
                    pText = &pLft;
                    boundRect = painter->boundingRect(pText->x(), pText->y()-22,40,24, flags, s) + QMargins(2,0,2,0);  //
                }


                painter->fillRect(boundRect, Qt::yellow);
                boundRect.setLeft(boundRect.x() + 2);
                painter->drawText(boundRect, s);

            }
        }
    }
}
