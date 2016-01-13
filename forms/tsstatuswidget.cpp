#include "QPainter"
#include "tsstatuswidget.h"


TsStatusWidget::TsStatusWidget(QWidget *parent) : QWidget(parent)
{
    page = 1;
    pSt = nullptr;
}

TsStatusWidget::~TsStatusWidget()
{

}

void TsStatusWidget::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    DrawGrid(&p);
    DrawTs  (&p);
}

void TsStatusWidget::DrawGrid(QPainter *p)
{
    //p->fillRect();
    for (int i=0; i<=32; i++)
    {
        p->setPen(i%8 ? Qt::gray : Qt::black);
        p->drawLine(0,dxy*i,dxy*32,dxy*i);
        p->drawLine(dxy*i,0,dxy*i,dxy*32);
    }
}

void TsStatusWidget::Update (class Station * pst, int p)
{
    pSt = pst;
    page = p;
    if (pSt)
    {

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

        QRect r(col * 16 + 2, row * 16 + 2, 12, 13);

        QColor bck(211,211,211);                                 //  = palette().background().color();
        QBrush brush(bck);
        p->setPen(bck);
        p->setBrush(brush);
        if (pSt==nullptr)
        {
            p->drawRect(r);
            continue;
        }

        // Учтем номер страницы
        bool sts        = false,
             stsPulse   = false;
        indxByte += (page - 1) * (1024 / 8);
        int indxBit = indxByte*8 + j;

        Ts * ts = pSt->GetTsByIndex(indxBit);
        sts      = ts!=nullptr ? ts->Sts()      : pSt->GetTsStsByIndex(indxBit);
        stsPulse = ts!=nullptr ? ts->StsPulse() : pSt->GetTsPulseStsByIndex(indxBit);

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
                     ts->Locked()    ? Qt::white :
                     ts->IsVirtual() ? Qt::blue  :
                     ts->IsReal()    ? Qt::darkGreen : Qt::darkGray;
        p->setPen(clr);

        if (sts==false && stsPulse==false)                      // 0
        {
            p->drawRect(r);
        }
        else
        if (sts && stsPulse==false)                              // 1
        {
            if (ts==nullptr)
                clr = (indxByte % 8) == (j % 8) ? Qt::darkGray : Qt::yellow;
            p->setPen(clr);
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
    }
}
