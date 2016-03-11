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

                    if (!shaperc->isStrlOk())               // проверка вхождения по стрелкам
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

                QFont font = QFont("Consolas",12, 60);
                painter->setFont(font);
                QString s = train->no ? QString::number(train->no) : QString::number(train->sno);
                QTextOption option = QTextOption(Qt::AlignLeft);
                painter->drawText(pLft, s);

//                    Train.No > 0 ? Train.NoTrainAndSfx : Train.SNo.ToString(),
//                    CultureInfo.GetCultureInfo("ru-RU"),
//                    FlowDirection.LeftToRight,
//                    new Typeface("Arial"), // "Consolas"
//                    16,
//                    Train.No > 0 ? new SolidColorBrush(Colors.White) : new SolidColorBrush(Colors.Black));
//                text.SetFontWeight(FontWeights.Bold);

//                double extrax = 8, extray = -2;
//                pLft.X += 8;
//                pRht.X -= 8;
//                //dc.PushOpacity(0.8);

//                Point pText, pBack;
//                if ((Train.IsEvn && !orient) || (!Train.IsEvn && orient))
//                {
//                    pText = pRht - new Vector(extrax/2 + text.Width, text.Height + extray + 3);
//                        // базовая точка текста
//                    pBack = pText - new Vector(extrax/2, extray/2); // базовая точка фона
//                    // направление >
//                    dc.DrawGeometry(TrainBrush, null,
//                                    new PathGeometry(ParsePathStr(pBack + new Vector(text.Width + extrax, 0),
//                                                                  "M9,9 l-8,-8 v16 Z")));
//                }
//                else
//                {
//                    pText = pLft - new Vector(-extrax/2, text.Height + extray + 3); // базовая точка текста
//                    pBack = pText - new Vector(extrax/2, extray/2); // базовая точка фона
//                    // направление <
//                    dc.DrawGeometry(TrainBrush, null, new PathGeometry(ParsePathStr(pBack, "M-9,9 l8,-8 v16 Z")));
//                }

//                // дорисовываем фон и текст
//                dc.DrawRectangle(TrainBrush, null,
//                                 new Rect(pBack.X, pBack.Y, text.Width + extrax, text.Height + extray));
//                dc.DrawText(text, pText);
//                //dc.Pop();
            }
        }
    }
}
