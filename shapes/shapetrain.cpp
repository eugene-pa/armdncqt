#include "shapetrain.h"
#include "../spr/train.h"
#include "../spr/rc.h"

ShapeTrain::ShapeTrain()
{

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
    foreach (Train train, Train::Trains)
    {
/*
        if (bShowTrains && (bShowNonregTrains || Train.No > 0))
        {
            Point pLft = new Point(99999, 99999),
                  // крайняя левая точка
                  pRht = new Point(0, 0); // крайняя правая точка

            bool orientLft = true, orientRht = true;
            bool found = false;

            foreach (RcInfo rc in Train.DcActualRcList)
            {
                foreach (Shape shape in rc.Shapes)
                {
                    if (!(shape is ShapeRc))
                        continue; // нонсенс

                    ShapeRc shapeRc = shape as ShapeRc;
                    if (shapeRc.SprRc == null || shapeRc.SprRc.SprSt == null)
                        continue; // пропускаем неопределенные

                    if (!shapeRc.AllStrlOk) // проверка вхождения по стрелкам
                        continue;

                    found = true;
                    if (pLft.X > shapeRc.X1) // проверяю оба конца отрезка на случай перевернутых отрезков
                    {
                        pLft = shapeRc.XY; // X1 левее pLft
                        orientLft = shapeRc.SprSt.IsOrientEvnOdd;
                    }
                    if (pLft.X > shapeRc.X2) // X2 левее pLft
                    {
                        pLft = shapeRc.X2Y2;
                        orientLft = shapeRc.SprSt.IsOrientEvnOdd;
                    }
                    if (pRht.X < shapeRc.X1)
                    {
                        pRht = shapeRc.XY; // X1 правее pRht
                        orientRht = !shapeRc.SprSt.IsOrientEvnOdd;      // 2015.08.21. инверсия
                    }
                    if (pRht.X < shapeRc.X2) // X2 правее pRht
                    {
                        pRht = shapeRc.X2Y2;
                        orientRht = !shapeRc.SprSt.IsOrientEvnOdd;      // 2015.08.21. инверсия
                    }
                }
            }
            if (found)
            {
                bool orient = orientLft == orientRht ? orientLft : orientRht;

                FormattedText text = new FormattedText(
                    Train.No > 0 ? Train.NoTrainAndSfx : Train.SNo.ToString(),
                    CultureInfo.GetCultureInfo("ru-RU"),
                    FlowDirection.LeftToRight,
                    new Typeface("Arial"), // "Consolas"
                    16,
                    Train.No > 0 ? new SolidColorBrush(Colors.White) : new SolidColorBrush(Colors.Black));
                text.SetFontWeight(FontWeights.Bold);

                double extrax = 8, extray = -2;
                pLft.X += 8;
                pRht.X -= 8;
                //dc.PushOpacity(0.8);

                Point pText, pBack;
                if ((Train.IsEvn && !orient) || (!Train.IsEvn && orient))
                {
                    pText = pRht - new Vector(extrax/2 + text.Width, text.Height + extray + 3);
                        // базовая точка текста
                    pBack = pText - new Vector(extrax/2, extray/2); // базовая точка фона
                    // направление >
                    dc.DrawGeometry(TrainBrush, null,
                                    new PathGeometry(ParsePathStr(pBack + new Vector(text.Width + extrax, 0),
                                                                  "M9,9 l-8,-8 v16 Z")));
                }
                else
                {
                    pText = pLft - new Vector(-extrax/2, text.Height + extray + 3); // базовая точка текста
                    pBack = pText - new Vector(extrax/2, extray/2); // базовая точка фона
                    // направление <
                    dc.DrawGeometry(TrainBrush, null, new PathGeometry(ParsePathStr(pBack, "M-9,9 l8,-8 v16 Z")));
                }

                // дорисовываем фон и текст
                dc.DrawRectangle(TrainBrush, null,
                                 new Rect(pBack.X, pBack.Y, text.Width + extrax, text.Height + extray));
                dc.DrawText(text, pText);
                //dc.Pop();
            }
        }

*/
    }
}
