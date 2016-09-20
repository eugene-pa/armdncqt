#include "QPainter"
#include "QMouseEvent"
#include "QToolTip"
#include "tsstatuswidget.h"


TsStatusWidget::TsStatusWidget(QWidget *parent) : QWidget(parent)
{
    dxy = 16;
    page = 1;
    st = nullptr;
    normal = false;
    actualNode = -1;
    setMouseTracking(true);

    startTimer(1500);
}

TsStatusWidget::~TsStatusWidget()
{

}

void TsStatusWidget::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e)
    QPainter p(this);
    DrawGrid(&p);
    if (st != nullptr)
        DrawTs  (&p);
}

void TsStatusWidget::DrawGrid(QPainter *p)
{
    for (int i=0; i<=32; i++)
    {
        p->setPen(i%8 ? Qt::gray : Qt::black);
        p->drawLine(0,dxy*i,dxy*32,dxy*i);
        p->drawLine(dxy*i,0,dxy*i,dxy*32);
    }
    if (actualNode >=0)
    {
        int row         = actualNode / 32,                  // строка в матрице
            col         = actualNode % 32;                  // столбец в матрице
        QRect r(col * 16+1, row * 16+1, 15, 15);
        p->setBrush(Qt::transparent);
        p->setPen(QPen(Qt::cyan, 2));
        p->drawRect(r);
    }
}

void TsStatusWidget::updateWidget (class Station * pst, int p)
{
    if (st != pst || page != p)
    {
        st = pst;
        page = p;
        update();
    }
}

void TsStatusWidget::DrawTs  (QPainter *p)
{
    for (int i = 0; i < 32*32 ; i++)
    {
        int row         = i / 32,                               // строка в матрице
            col         = i % 32,                               // столбец в матрице
            indxByte    = i / 8,                                // индекс байта в массиве
            j           = (i % 8);                              // номер бита в байте

        QRect r(col * 16 + 2, row * 16 + 2, 13, 13);

        QColor bck(211,211,211);                                 //  = palette().background().color();
        QBrush brush(bck);
        p->setPen(bck);
        p->setBrush(brush);
        if (st==nullptr)
        {
            p->drawRect(r);
            continue;
        }

        // Учтем номер страницы
        bool sts        = false,
             stsPulse   = false;

        indxByte += (page - 1) * (1024 / 8);
        int indxBit = indxByte*8 + j;

        Ts * ts = st->GetTsByIndex(indxBit);
        // состояние берем из массива ТС, так как в одном узле может быть несколько логических сигналов
        sts      = normal ? st->GetTsStsByIndex(indxBit) : st->GetTsStsRawByIndex(indxBit);
        stsPulse = /*ts!=nullptr ? ts->StsPulse() :*/ st->GetTsPulseStsByIndex(indxBit);

        // 0
        //   - светло-серый фон
        //     окантовка: - синий цвет для виртуальных
        //                - зеленый для описанных на МТС
        //                - серый для описанных от других систем
        // 1
        //   - синий цвет для виртуальных
        //   - зеленый для описанных на МТС
        //   - серый для описанных от других систем
        //   - желтый для неописанных
        // мигание
        //   - желтый для неописанных  и внутри маленький квадрат
        //   - светло-зеленый для МТС  и внутри маленький квадрат
        //   - серый для других систем и внутри маленький квадрат

        QColor clr = ts == nullptr   ? bck :
                     ts->Disabled()  ? Qt::white :
                     ts->IsVirtual() ? Qt::blue  :
                     ts->IsReal()    ? Qt::darkGreen : Qt::darkGray;
        p->setPen(clr);

        if (sts==false && stsPulse==false)                      // 0
        {
            r.setHeight(r.height()-1);
            r.setWidth(r.width()-1);
            p->drawRect(r);
        }
        else
        if (sts && stsPulse==false)                              // 1
        {
            if (ts==nullptr)
                clr = st->Kp2000() && (indxByte % 8) == (j % 8) ? Qt::darkGray : Qt::yellow;
            p->setPen(clr);
            p->setBrush(clr);
            p->fillRect(r, clr);
        }
        else                                                    // мигание
        {
            clr = ts==nullptr ? ((indxByte % 8) == (j % 8) ? Qt::darkGray : Qt::yellow) : Qt::darkGreen;
            p->setPen(clr);
            p->fillRect(r, clr);

            p->setPen(bck);
            p->setBrush(QBrush(bck));
            p->drawEllipse(r.center(),3,3);
        }

        if (ts!=nullptr)
        {
            // выделение сигналов с совпадающими местами в матрице окантовкой узла матрицы
            if (ts->GetNext() != nullptr)
            {
                p->setPen(Qt::green);
                p->drawRect(r.x() -1, r.y()-1, r.width() + 2, r.height() + 2);
            }

            // выделение сигналов с инверсией штрихом сверху
            if (ts->IsInverse())
            {
                p->setPen(QPen(Qt::black, 3));
                p->drawLine(r.topLeft() + QPoint(1,1),r.topRight() + QPoint(-1,1));
            }
        }
    }
}

void TsStatusWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (isVisible())
        this->update();
}

void TsStatusWidget::mousePressEvent(QMouseEvent * event)
{
    QPoint point = event->pos();
    actualNode = (point.y() / 16) * 32 + point.x() / 16;
    int no =  actualNode + 1 + (page - 1) * 1024;
    emit tsSelected (no);
    update();
}

void TsStatusWidget::mouseMoveEvent(QMouseEvent * event)
{
    QPoint point = event->pos();
    Ts * ts = st->GetTsByIndex((point.y() / 16) * 32 + point.x() / 16 + (page - 1) * 1024);
    QToolTip::showText(event->globalPos(), ts==nullptr ? "" : ts->GetTooltip());
}
